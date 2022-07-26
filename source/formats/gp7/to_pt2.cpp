/*
 * Copyright (C) 2020 Cameron White
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "to_pt2.h"
#include "document.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/rational.hpp>
#include <numeric>

#include <formats/fileformat.h>
#include <score/generalmidi.h>
#include <score/keysignature.h>
#include <score/note.h>
#include <score/playerchange.h>
#include <score/position.h>
#include <score/rehearsalsign.h>
#include <score/score.h>
#include <score/scoreinfo.h>
#include <score/system.h>
#include <score/tempomarker.h>
#include <score/timesignature.h>
#include <score/utils.h>
#include <score/utils/scorepolisher.h>
#include <score/voiceutils.h>
#include <util/tostring.h>

#include <iostream>
#include <unordered_set>

/// Convert the Guitar Pro file metadata.
static void
convertScoreInfo(const Gp7::ScoreInfo &gp_info, Score &score)
{
    ScoreInfo info;
    SongData data;

    data.setTitle(gp_info.myTitle);
    data.setSubtitle(gp_info.mySubtitle);
    data.setArtist(gp_info.myArtist);

    data.setAudioReleaseInfo(SongData::AudioReleaseInfo(
        SongData::AudioReleaseInfo::ReleaseType::Album, gp_info.myAlbum,
        boost::gregorian::day_clock::local_day().year(), false));

    // WordsAndMusic never seems to be set.
    data.setAuthorInfo(SongData::AuthorInfo(gp_info.myMusic, gp_info.myWords));
    data.setCopyright(gp_info.myCopyright);
    data.setTranscriber(gp_info.myTabber);
    data.setPerformanceNotes(gp_info.myInstructions);
    // Skipping Notices since there isn't an equivalent.

    info.setSongData(data);
    score.setScoreInfo(info);
}

/// Create players and instruments from the Guitar Pro tracks, and the initial
/// staff -> player & instrument assignment.
static void
convertPlayers(const std::vector<Gp7::Track> &tracks, Score &score,
               PlayerChange &player_change)
{
    for (const Gp7::Track &track : tracks)
    {
        assert(!track.mySounds.empty());
        const int instrument_idx = static_cast<int>(score.getInstruments().size());

        for (const Gp7::Sound &sound : track.mySounds)
        {
            Instrument instrument;
            instrument.setDescription(sound.myLabel);
            instrument.setMidiPreset(sound.myMidiPreset);

            score.insertInstrument(instrument);
        }

        for (const Gp7::Staff &staff : track.myStaves)
        {
            Player player;
            player.setDescription(track.myName);

            Tuning tuning;

            // In .gpx files, the drums don't have a tuning, so just leave it
            // at the default in that case.
            if (!staff.myTuning.empty())
            {
                tuning.setNotes(std::vector<uint8_t>(staff.myTuning.rbegin(),
                                                     staff.myTuning.rend()));
            }
            tuning.setCapo(staff.myCapo);
            player.setTuning(tuning);

            const int player_idx = static_cast<int>(score.getPlayers().size());
            score.insertPlayer(player);

            // Each player will be assigned its own staff, and is initially
            // assigned to the track's first instrument.
            // TODO - import track automations.
            player_change.insertActivePlayer(
                player_idx, ActivePlayer(player_idx, instrument_idx));
        }
    }
}

/// Convert from a Guitar Pro clef type to our clef type.
static Staff::ClefType
convertClefType(Gp7::Bar::ClefType clef_type)
{
    switch (clef_type)
    {
        case Gp7::Bar::ClefType::F4:
            return Staff::BassClef;
        default:
            return Staff::TrebleClef;
    }
}

/// Returns the pitch offset in half steps for a (relative) harmonic fret.
static int
getHarmonicPitchOffset(double harmonic_fret)
{
    auto approx_equal = [](double a, double b) {
        return std::abs(b - a) < 1e-5;
    };

    if (approx_equal(harmonic_fret, 12))
    {
        // Octave.
        return 12;
    }
    else if (approx_equal(harmonic_fret, 7) || approx_equal(harmonic_fret, 19))
    {
        // Octave + fifth.
        return 19;
    }
    else if (approx_equal(harmonic_fret, 5) || approx_equal(harmonic_fret, 24))
    {
        // 2nd octave.
        return 24;
    }
    else if (approx_equal(harmonic_fret, 4) || approx_equal(harmonic_fret, 9) ||
             approx_equal(harmonic_fret, 16))
    {
        // 2nd octave + third.
        return 28;
    }
    else if (approx_equal(harmonic_fret, 3.2))
    {
        // 2nd octave + fifth.
        return 31;
    }
    else if (approx_equal(harmonic_fret, 2.7) ||
             approx_equal(harmonic_fret, 5.8) ||
             approx_equal(harmonic_fret, 9.6) ||
             approx_equal(harmonic_fret, 14.7) ||
             approx_equal(harmonic_fret, 21.7))
    {
        // 2nd octave + minor 7th.
        return 34;
    }
    else if (approx_equal(harmonic_fret, 2.4) ||
             approx_equal(harmonic_fret, 8.2) ||
             approx_equal(harmonic_fret, 17))
    {
        // 3rd octave.
        return 36;
    }
    else
    {
        std::cerr << "Unexpected harmonic type" << std::endl;
        return 12;
    }
}

using KeyAndVariation = std::pair<ChordName::Key, ChordName::Variation>;

static KeyAndVariation
keyAndVariationFromPitch(int midi_pitch)
{
    static const KeyAndVariation theNotes[12] = {
        { ChordName::C, ChordName::NoVariation },
        { ChordName::C, ChordName::Sharp },
        { ChordName::D, ChordName::NoVariation },
        { ChordName::D, ChordName::Sharp },
        { ChordName::E, ChordName::NoVariation },
        { ChordName::F, ChordName::NoVariation },
        { ChordName::F, ChordName::Sharp },
        { ChordName::G, ChordName::NoVariation },
        { ChordName::G, ChordName::Sharp },
        { ChordName::A, ChordName::NoVariation },
        { ChordName::A, ChordName::Sharp },
        { ChordName::B, ChordName::NoVariation }
    };

    assert(midi_pitch >= 0 && midi_pitch < 12);
    return theNotes[midi_pitch];
}

/// Convert Guitar Pro's bend values (a float providing the percentage of full
/// steps) to our bend values, which just store the number of quarter steps.
static int
convertPitchToQuarterStep(double value)
{
    return static_cast<int>(value * 0.04);
}

static Note
convertNote(Position &position, const Gp7::Beat &gp_beat,
            const Gp7::Note &gp_note, const Tuning &tuning)
{
    using HarmonicType = Gp7::Note::HarmonicType;

    Note note;
    note.setFretNumber(gp_note.myFret);
    // String numbers are flipped around.
    note.setString(tuning.getStringCount() - gp_note.myString - 1);

    note.setProperty(Note::Tied, gp_note.myTieDest);
    note.setProperty(Note::GhostNote, gp_note.myGhost);
    note.setProperty(Note::Muted, gp_note.myMuted);
    note.setProperty(Note::HammerOnOrPullOff, gp_note.myHammerOn);
    note.setProperty(Note::HammerOnFromNowhere, gp_note.myLeftHandTapped);

    if (gp_note.myPalmMuted)
        position.setProperty(Position::PalmMuting);
    if (gp_note.myTapped)
        position.setProperty(Position::Tap);
    if (gp_note.myVibrato)
        position.setProperty(Position::Vibrato);
    if (gp_note.myWideVibrato)
        position.setProperty(Position::WideVibrato);
    if (gp_note.myLetRing)
        position.setProperty(Position::LetRing);

    // Staccatissimo and tenuto are not supported.
    using GpAccentType = Gp7::Note::AccentType;
    if (gp_note.myAccentTypes.test(int(GpAccentType::Staccato)))
        position.setProperty(Position::Staccato);
    if (gp_note.myAccentTypes.test(int(GpAccentType::HeavyAccent)))
        position.setProperty(Position::Sforzando);
    if (gp_note.myAccentTypes.test(int(GpAccentType::Accent)))
        position.setProperty(Position::Marcato);

    using GpSlideType = Gp7::Note::SlideType;
    if (gp_note.mySlideTypes.test(int(GpSlideType::Shift)))
        note.setProperty(Note::ShiftSlide);
    if (gp_note.mySlideTypes.test(int(GpSlideType::Legato)))
        note.setProperty(Note::LegatoSlide);
    if (gp_note.mySlideTypes.test(int(GpSlideType::SlideOutDown)))
        note.setProperty(Note::SlideOutOfDownwards);
    if (gp_note.mySlideTypes.test(int(GpSlideType::SlideOutUp)))
        note.setProperty(Note::SlideOutOfUpwards);
    if (gp_note.mySlideTypes.test(int(GpSlideType::SlideInAbove)))
        note.setProperty(Note::SlideIntoFromAbove);
    if (gp_note.mySlideTypes.test(int(GpSlideType::SlideInBelow)))
        note.setProperty(Note::SlideIntoFromBelow);

    if (gp_beat.myOttavia)
    {
        switch (*gp_beat.myOttavia)
        {
            case Gp7::Beat::Ottavia::O8va:
                note.setProperty(Note::Octave8va);
                break;
            case Gp7::Beat::Ottavia::O8vb:
                note.setProperty(Note::Octave8vb);
                break;
            case Gp7::Beat::Ottavia::O15ma:
                note.setProperty(Note::Octave15ma);
                break;
            case Gp7::Beat::Ottavia::O15mb:
                note.setProperty(Note::Octave15mb);
                break;
        }
    }

    if (gp_note.myHarmonic)
    {
        note.setProperty(Note::NaturalHarmonic,
                         gp_note.myHarmonic == HarmonicType::Natural);

        if (gp_note.myHarmonic == HarmonicType::Tap)
        {
            // Note that we don't support harmonics at fractional frets.
            note.setTappedHarmonicFret(
                note.getFretNumber() +
                static_cast<int>(gp_note.myHarmonicFret));
        }

        if (gp_note.myHarmonic == HarmonicType::Artificial ||
            gp_note.myHarmonic == HarmonicType::Pinch ||
            gp_note.myHarmonic == HarmonicType::Semi ||
            gp_note.myHarmonic == HarmonicType::Feedback)
        {
            // Convert the fret offset into a key/variation plus an octave
            // offset.
            int midi_note = tuning.getNote(note.getString(), false) +
                            tuning.getCapo() + note.getFretNumber();
            const int offset = getHarmonicPitchOffset(gp_note.myHarmonicFret);
            midi_note += offset;

            const int pitch = Midi::getMidiNotePitch(midi_note);
            auto [key, variation] = keyAndVariationFromPitch(pitch);

            const int octave_diff =
                Midi::getMidiNoteOctave(midi_note) -
                Midi::getMidiNoteOctave(midi_note - offset) - 1;

            assert(octave_diff >= 0 && octave_diff <= 2);
            auto octave_type =
                static_cast<ArtificialHarmonic::Octave>(octave_diff);

            note.setArtificialHarmonic(
                ArtificialHarmonic(key, variation, octave_type));
        }
    }

    // The GP7 trill stores the note value, not the fret number.
    if (gp_note.myTrillNote)
    {
        note.setTrilledFret(*gp_note.myTrillNote -
                            tuning.getNote(note.getString(), false));
    }

    if (gp_note.myLeftFinger)
    {
        using Finger = LeftHandFingering::Finger;
        using FingerType = Gp7::Note::FingerType;
        switch (*gp_note.myLeftFinger)
        {
            case FingerType::Open:
                note.setLeftHandFingering(LeftHandFingering(Finger::None));
                break;
            case FingerType::C:
                note.setLeftHandFingering(LeftHandFingering(Finger::Little));
                break;
            case FingerType::A:
                note.setLeftHandFingering(LeftHandFingering(Finger::Ring));
                break;
            case FingerType::M:
                note.setLeftHandFingering(LeftHandFingering(Finger::Middle));
                break;
            case FingerType::I:
                note.setLeftHandFingering(LeftHandFingering(Finger::Index));
                break;
            case FingerType::P:
                note.setLeftHandFingering(LeftHandFingering(Finger::Thumb));
                break;
        }
    }

    if (gp_note.myBend)
    {
        const Gp7::Bend &gp_bend = *gp_note.myBend;

        Bend::BendType bend_type = Bend::NormalBend;
        int start_pitch = convertPitchToQuarterStep(gp_bend.myOriginValue);
        int end_pitch = convertPitchToQuarterStep(gp_bend.myDestValue);
        int middle_pitch = convertPitchToQuarterStep(gp_bend.myMiddleValue);
        const bool has_middle_pitch =
            (gp_bend.myMiddleOffset1 != gp_bend.myMiddleOffset2);

        int bent_pitch = -1;
        int release_pitch = -1;

        if (!has_middle_pitch)
        {
            bend_type = Bend::NormalBend;
            if (start_pitch > 0)
            {
                if (gp_note.myTieDest && !gp_note.myTieOrigin)
                    bend_type = Bend::GradualRelease;
                else
                {
                    bend_type = (end_pitch != start_pitch)
                                    ? Bend::PreBendAndRelease
                                    : Bend::PreBend;
                }

                bent_pitch = start_pitch;
                release_pitch = end_pitch;
            }
            else
                bent_pitch = end_pitch;

            if (gp_note.myTieOrigin)
            {
                if (start_pitch > 0)
                    bend_type = Bend::PreBendAndHold;
                else
                    bend_type = Bend::BendAndHold;
            }
        }
        else
        {
            bend_type = Bend::BendAndRelease;
            bent_pitch = middle_pitch;
            release_pitch = end_pitch;
        }

        Bend::DrawPoint start_point =
            start_pitch <= end_pitch ? Bend::LowPoint : Bend::MidPoint;
        Bend::DrawPoint end_point =
            start_pitch <= end_pitch ? Bend::MidPoint : Bend::LowPoint;
        note.setBend(Bend(bend_type, bent_pitch, release_pitch, 0, start_point,
                          end_point));
    }

    return note;
}

static Position
convertPosition(const Gp7::Beat &gp_beat, const Gp7::Rhythm &gp_rhythm)
{
    Position pos;

    pos.setDurationType(
        static_cast<Position::DurationType>(gp_rhythm.myDuration));
    pos.setProperty(Position::Dotted, gp_rhythm.myDots == 1);
    pos.setProperty(Position::DoubleDotted, gp_rhythm.myDots == 2);

    pos.setProperty(Position::Acciaccatura, gp_beat.myGraceNote);
    pos.setProperty(Position::TremoloPicking, gp_beat.myTremoloPicking);

    pos.setProperty(Position::PickStrokeDown, gp_beat.myBrushDown);
    pos.setProperty(Position::PickStrokeUp, gp_beat.myBrushUp);

    pos.setProperty(Position::ArpeggioUp, gp_beat.myArpeggioUp);
    pos.setProperty(Position::ArpeggioDown, gp_beat.myArpeggioDown);

    if (gp_beat.myWhammy)
    {
        const Gp7::Bend &whammy = *gp_beat.myWhammy;

        TremoloBar::Type type = TremoloBar::Type::DiveAndRelease;
        int start_pitch = convertPitchToQuarterStep(whammy.myOriginValue);
        int end_pitch = convertPitchToQuarterStep(whammy.myDestValue);
        int middle_pitch = convertPitchToQuarterStep(whammy.myMiddleValue);
        const bool is_dip =
            (middle_pitch < start_pitch && end_pitch > middle_pitch) ||
            (middle_pitch > start_pitch && end_pitch < middle_pitch);

        int pitch = -1;
        if (!is_dip)
        {
            // TODO - this can probably be improved to find "hold" events,
            // perhaps by checking if the next beat has a whammy.
            pitch = std::abs(end_pitch);
            if (end_pitch < start_pitch)
                type = TremoloBar::Type::DiveAndRelease;
            else
                type = TremoloBar::Type::ReturnAndRelease;
        }
        else
        {
            pitch = std::abs(middle_pitch);
            if (middle_pitch < end_pitch)
                type = TremoloBar::Type::Dip;
            else
                type = TremoloBar::Type::InvertedDip;
        }

        pos.setTremoloBar(TremoloBar(type, pitch, 0));
    }

    return pos;
}

static void
convertDirections(System &system, int start_pos, int end_bar_pos,
                  const Gp7::MasterBar &master_bar)
{
    using DirectionTarget = Gp7::MasterBar::DirectionTarget;
    using DirectionJump = Gp7::MasterBar::DirectionJump;

    Direction start_dir{ start_pos };
    Direction end_dir{ end_bar_pos };

    for (DirectionTarget target : master_bar.myDirectionTargets)
    {
        switch (target)
        {
            case DirectionTarget::Fine:
                end_dir.insertSymbol(DirectionSymbol::Fine);
                break;
            case DirectionTarget::Coda:
                start_dir.insertSymbol(DirectionSymbol::Coda);
                break;
            case DirectionTarget::DoubleCoda:
                start_dir.insertSymbol(DirectionSymbol::DoubleCoda);
                break;
            case DirectionTarget::Segno:
                start_dir.insertSymbol(DirectionSymbol::Segno);
                break;
            case DirectionTarget::SegnoSegno:
                start_dir.insertSymbol(DirectionSymbol::SegnoSegno);
                break;
        }
    }

    for (DirectionJump jump : master_bar.myDirectionJumps)
    {
        switch (jump)
        {
            case DirectionJump::DaCapo:
                end_dir.insertSymbol(DirectionSymbol::DaCapo);
                break;
            case DirectionJump::DaCapoAlCoda:
                end_dir.insertSymbol(DirectionSymbol::DaCapoAlCoda);
                break;
            case DirectionJump::DaCapoAlDoubleCoda:
                end_dir.insertSymbol(DirectionSymbol::DaCapoAlDoubleCoda);
                break;
            case DirectionJump::DaCapoAlFine:
                end_dir.insertSymbol(DirectionSymbol::DaCapoAlFine);
                break;
            case DirectionJump::DaSegno:
                end_dir.insertSymbol(DirectionSymbol::DalSegno);
                break;
            case DirectionJump::DaSegnoAlCoda:
                end_dir.insertSymbol(DirectionSymbol::DalSegnoAlCoda);
                break;
            case DirectionJump::DaSegnoAlDoubleCoda:
                end_dir.insertSymbol(DirectionSymbol::DalSegnoAlDoubleCoda);
                break;
            case DirectionJump::DaSegnoAlFine:
                end_dir.insertSymbol(DirectionSymbol::DalSegnoAlFine);
                break;
            case DirectionJump::DaSegnoSegno:
                end_dir.insertSymbol(DirectionSymbol::DalSegnoSegno);
                break;
            case DirectionJump::DaSegnoSegnoAlCoda:
                end_dir.insertSymbol(DirectionSymbol::DalSegnoSegnoAlCoda);
                break;
            case DirectionJump::DaSegnoSegnoAlDoubleCoda:
                end_dir.insertSymbol(
                    DirectionSymbol::DalSegnoSegnoAlDoubleCoda);
                break;
            case DirectionJump::DaSegnoSegnoAlFine:
                end_dir.insertSymbol(DirectionSymbol::DalSegnoSegnoAlFine);
                break;
            case DirectionJump::DaCoda:
                end_dir.insertSymbol(DirectionSymbol::ToCoda);
                break;
            case DirectionJump::DaDoubleCoda:
                end_dir.insertSymbol(DirectionSymbol::ToDoubleCoda);
                break;
        }
    }

    if (!start_dir.getSymbols().empty())
        system.insertDirection(start_dir);
    if (!end_dir.getSymbols().empty())
        system.insertDirection(end_dir);
}

static void
convertAlternateEndings(System &system, int bar_pos,
                        const Gp7::MasterBar &master_bar)
{
    if (master_bar.myAlternateEndings.empty())
        return;

    AlternateEnding ending;
    for (int number : master_bar.myAlternateEndings)
        ending.addNumber(number);

    ending.setPosition(bar_pos);
    system.insertAlternateEnding(ending);
}

static void
convertTempoMarkers(System &system, int bar_pos,
                    const Gp7::MasterBar &master_bar)
{
    if (master_bar.myTempoChanges.empty())
        return;

    // TODO - there can be multiple tempo changes at different locations within
    // the bar, which don't necessarily correspond to a beat. For now, just
    // place it at the start of the bar to handle the common cases.
    // Alterations of pace are also not translated yet.
    const Gp7::TempoChange &gp_tempo = master_bar.myTempoChanges[0];

    TempoMarker marker;
    switch (gp_tempo.myBeatType)
    {
        using BeatType = Gp7::TempoChange::BeatType;

        case BeatType::Eighth:
            marker.setBeatType(TempoMarker::Eighth);
            break;
        case BeatType::Half:
            marker.setBeatType(TempoMarker::Half);
            break;
        case BeatType::HalfDotted:
            marker.setBeatType(TempoMarker::HalfDotted);
            break;
        case BeatType::Quarter:
            marker.setBeatType(TempoMarker::Quarter);
            break;
        case BeatType::QuarterDotted:
            marker.setBeatType(TempoMarker::QuarterDotted);
            break;
    }

    marker.setBeatsPerMinute(gp_tempo.myBeatsPerMinute);
    marker.setDescription(gp_tempo.myDescription);

    marker.setPosition(bar_pos);
    system.insertTempoMarker(marker);
}

static void
convertBarline(Barline &start_bar, Barline &end_bar,
               const Gp7::MasterBar &master_bar,
               const Gp7::MasterBar *prev_master_bar, bool final_bar)
{
    if (master_bar.mySection)
    {
        // Note that ScoreUtils::adjustRehearsalSigns() will re-assign the
        // letters later.
        start_bar.setRehearsalSign(RehearsalSign(master_bar.mySection->myLetter,
                                                 master_bar.mySection->myText));
    }

    if (master_bar.myRepeatEnd)
    {
        end_bar.setBarType(Barline::RepeatEnd);
        end_bar.setRepeatCount(master_bar.myRepeatCount);
    }
    else if (final_bar)
        end_bar.setBarType(Barline::DoubleBarFine);
    else if (master_bar.myDoubleBar)
        end_bar.setBarType(Barline::DoubleBar);
    else if (master_bar.myFreeTime)
        end_bar.setBarType(Barline::FreeTimeBar);

    if (master_bar.myRepeatStart)
        start_bar.setBarType(Barline::RepeatStart);

    {
        TimeSignature time_sig;
        time_sig.setBeatsPerMeasure(master_bar.myTimeSig.myBeats);
        time_sig.setNumPulses(master_bar.myTimeSig.myBeats);
        time_sig.setBeatValue(master_bar.myTimeSig.myBeatValue);

        // Set the time signature on the end bar, for consistency, but leave it
        // invisible.
        end_bar.setTimeSignature(time_sig);

        // Display the time signature for the first bar in the score, or if
        // there was a time signature change.
        if (!prev_master_bar ||
            (prev_master_bar->myTimeSig != master_bar.myTimeSig))
        {
            time_sig.setVisible();
        }

        start_bar.setTimeSignature(time_sig);
    }

    {
        KeySignature key_sig;
        key_sig.setNumAccidentals(master_bar.myKeySig.myAccidentalCount);
        key_sig.setKeyType(master_bar.myKeySig.myMinor ? KeySignature::Minor
                                                       : KeySignature::Major);
        key_sig.setSharps(master_bar.myKeySig.mySharps);

        // Set the key signature on the end bar, for consistency, but leave it
        // invisible.
        end_bar.setKeySignature(key_sig);

        // Display the key signature for the first bar in the system, or if
        // there was a key signature change.
        const bool changed = (prev_master_bar &&
                              prev_master_bar->myKeySig != master_bar.myKeySig);
        if (start_bar.getPosition() == 0 || changed)
        {
            key_sig.setVisible();

            // Set up a cancellation if necessary.
            if (changed && key_sig.getNumAccidentals() == 0 &&
                prev_master_bar->myKeySig.myAccidentalCount != 0)
            {
                key_sig.setCancellation();
                key_sig.setNumAccidentals(
                    prev_master_bar->myKeySig.myAccidentalCount);
                key_sig.setSharps(prev_master_bar->myKeySig.mySharps);
            }
        }

        start_bar.setKeySignature(key_sig);
    }
}

/// Convert irregular groups for a single bar.
static void
convertIrregularGroupings(Voice &voice, int start_pos, int end_pos,
                          const Gp7::Document &doc, const Gp7::Voice &gp_voice)
{
    std::optional<IrregularGrouping> current_group;
    boost::rational<int> duration;
    int division = -1;

    auto positions =
        ScoreUtils::findInRange(voice.getPositions(), start_pos, end_pos);

    auto it = positions.begin();
    for (int gp_beat_idx : gp_voice.myBeatIds)
    {
        const Gp7::Beat &gp_beat = doc.myBeats.at(gp_beat_idx);
        const Gp7::Rhythm &gp_rhythm = doc.myRhythms.at(gp_beat.myRhythmId);
        const Position &pos = *it;

        if (gp_rhythm.myTupletDenom)
        {
            // Check if we need to start a new group.
            if (!current_group ||
                (current_group->getNotesPlayed() != gp_rhythm.myTupletNum &&
                 current_group->getNotesPlayedOver() !=
                     gp_rhythm.myTupletDenom))
            {
                current_group = IrregularGrouping(pos.getPosition(), 0,
                                                  gp_rhythm.myTupletNum,
                                                  gp_rhythm.myTupletDenom);
                duration = 0;
                division = 0;
            }

            // Accumulate this note's duration, and track which note division
            // we're using (8, 16, etc).
            division = std::max(division, gp_rhythm.myDuration);
            duration += VoiceUtils::getDurationTime(voice, pos);
            current_group->setLength(current_group->getLength() + 1);

            // The duration is in quarter notes, so figure out if we have
            // enough notes of the division we're using!
            const int num_notes =
                (duration * (division / Position::QuarterNote)).numerator();
            if ((num_notes % current_group->getNotesPlayed()) == 0)
            {
                voice.insertIrregularGrouping(*current_group);
                current_group.reset();
            }
        }
        else
        {
            // Didn't find enough notes?
            current_group.reset();
        }

        ++it;
    }
}

static ChordName::Key
convertChordKey(const std::string &name)
{
    static const std::unordered_map<std::string, ChordName::Key> theKeyNames = {
        { "C", ChordName::C }, { "D", ChordName::D }, { "E", ChordName::E },
        { "F", ChordName::F }, { "G", ChordName::G }, { "A", ChordName::A },
        { "B", ChordName::B },
    };

    auto it = theKeyNames.find(name);
    assert(it != theKeyNames.end());
    return it->second;
}

static ChordName
convertChordName(const Gp7::ChordName &gp_chord)
{
    using Alteration = Gp7::ChordName::Degree::Alteration;

    ChordName chord;

    chord.setTonicKey(convertChordKey(gp_chord.myKeyNote.myStep));
    chord.setTonicVariation(
        static_cast<ChordName::Variation>(gp_chord.myKeyNote.myAccidental));

    chord.setBassKey(convertChordKey(gp_chord.myBassNote.myStep));
    chord.setBassVariation(
        static_cast<ChordName::Variation>(gp_chord.myBassNote.myAccidental));

    if (!gp_chord.myFifth)
        return chord;

    auto fifth = gp_chord.myFifth->myAlteration;
    if (gp_chord.mySeventh && gp_chord.myThird)
    {
        auto seventh = gp_chord.mySeventh->myAlteration;
        auto third = gp_chord.myThird->myAlteration;

        if (seventh == Alteration::Minor)
        {
            if (third == Alteration::Minor)
            {
                if (fifth == Alteration::Perfect)
                    chord.setFormula(ChordName::Minor7th);
                else if (fifth == Alteration::Diminished)
                    chord.setFormula(ChordName::Minor7thFlatted5th);
            }
            else if (third == Alteration::Major)
            {
                if (fifth == Alteration::Perfect)
                    chord.setFormula(ChordName::Dominant7th);
                else if (fifth == Alteration::Augmented)
                    chord.setFormula(ChordName::Augmented7th);
                else if (fifth == Alteration::Diminished)
                {
                    chord.setFormula(ChordName::Dominant7th);
                    chord.setModification(ChordName::Flatted5th);
                }
            }
        }
        else if (seventh == Alteration::Major)
        {
            if (third == Alteration::Minor && fifth == Alteration::Perfect)
            {
                chord.setFormula(ChordName::MinorMajor7th);
            }
            else if (third == Alteration::Major)
            {
                chord.setFormula(ChordName::Major7th);
            }
        }
        else if (seventh == Alteration::Diminished &&
                 fifth == Alteration::Diminished && third == Alteration::Minor)
        {
            chord.setFormula(ChordName::Diminished7th);
        }
    }
    else if (gp_chord.mySeventh && (gp_chord.mySecond || gp_chord.myFourth))
    {
        auto seventh = gp_chord.mySeventh->myAlteration;
        if (seventh == Alteration::Minor)
            chord.setFormula(ChordName::Dominant7th);
        else
            chord.setFormula(ChordName::Major7th);

        if (gp_chord.mySecond)
            chord.setModification(ChordName::Suspended2nd);
        else if (gp_chord.myFourth)
            chord.setModification(ChordName::Suspended4th);
    }
    else if (gp_chord.mySixth &&
             gp_chord.mySixth->myAlteration == Alteration::Major &&
             gp_chord.myThird)
    {
        auto third = gp_chord.myThird->myAlteration;
        if (third == Alteration::Major && fifth == Alteration::Perfect)
            chord.setFormula(ChordName::Major6th);
        else if (third == Alteration::Minor && fifth == Alteration::Perfect)
            chord.setFormula(ChordName::Minor6th);
    }
    else if (gp_chord.myThird)
    {
        auto third = gp_chord.myThird->myAlteration;

        if (third == Alteration::Major)
        {
            if (fifth == Alteration::Perfect)
                chord.setFormula(ChordName::Major);
            else if (fifth == Alteration::Augmented)
                chord.setFormula(ChordName::Augmented);
        }
        else if (third == Alteration::Minor)
        {
            if (fifth == Alteration::Perfect)
                chord.setFormula(ChordName::Minor);
            else if (fifth == Alteration::Diminished)
                chord.setFormula(ChordName::Diminished);
        }
    }
    else if (gp_chord.mySecond)
    {
        chord.setFormula(ChordName::Major);
        chord.setModification(ChordName::Suspended2nd);
    }
    else if (gp_chord.myFourth)
    {
        chord.setFormula(ChordName::Major);
        chord.setModification(ChordName::Suspended4th);
    }
    else
    {
        chord.setFormula(ChordName::PowerChord);
    }

    if (chord.getFormula() != ChordName::Augmented &&
        chord.getFormula() != ChordName::Augmented7th &&
        fifth == Alteration::Augmented)
    {
        chord.setModification(ChordName::Raised5th);
    }

    if (chord.getFormula() != ChordName::Major6th &&
        chord.getFormula() != ChordName::Minor6th && gp_chord.mySixth)
    {
        chord.setModification(
            (gp_chord.mySixth->myAlteration == Alteration::Minor)
                ? ChordName::Flatted6th
                : ChordName::Added6th);
    }

    if (gp_chord.myThird && gp_chord.mySecond)
        chord.setModification(ChordName::Added2nd);
    if (gp_chord.myThird && gp_chord.myFourth)
        chord.setModification(ChordName::Added4th);

    const bool is_seventh = chord.getFormula() >= ChordName::Dominant7th;

    if (gp_chord.myNinth)
    {
        auto ninth = gp_chord.myNinth->myAlteration;
        if (ninth == Alteration::Perfect)
        {
            if (is_seventh)
                chord.setModification(ChordName::Extended9th);
            else
                chord.setModification(ChordName::Added9th);
        }
        else if (ninth == Alteration::Diminished)
            chord.setModification(ChordName::Flatted9th);
        else if (ninth == Alteration::Augmented)
            chord.setModification(ChordName::Raised9th);
    }

    if (gp_chord.myEleventh)
    {
        auto eleventh = gp_chord.myEleventh->myAlteration;
        if (eleventh == Alteration::Perfect)
        {
            if (is_seventh)
                chord.setModification(ChordName::Extended11th);
            else
                chord.setModification(ChordName::Added11th);
        }
        else if (eleventh == Alteration::Augmented)
            chord.setModification(ChordName::Raised11th);
    }

    if (gp_chord.myThirteenth)
    {
        auto thirteenth = gp_chord.myThirteenth->myAlteration;
        if (thirteenth == Alteration::Perfect && is_seventh)
            chord.setModification(ChordName::Extended13th);
        else if (thirteenth == Alteration::Diminished)
            chord.setModification(ChordName::Flatted13th);
    }

    return chord;
}

static ChordDiagram
convertChordDiagram(const Gp7::Chord &gp_chord)
{
    ChordDiagram diagram;
    diagram.setChordName(convertChordName(gp_chord.myName));
    diagram.setTopFret(gp_chord.myDiagram.myBaseFret);

    std::vector<int> frets = gp_chord.myDiagram.myFrets;
    // Strings are in the opposite order, and frets are relative to the top
    // fret.
    std::reverse(frets.begin(), frets.end());
    for (int &fret : frets)
    {
        if (fret > 0)
            fret += diagram.getTopFret();
    }

    diagram.setFretNumbers(frets);
    return diagram;
}

static void
convertChordDiagrams(const Gp7::Document &doc, Score &score)
{
    std::unordered_set<ChordDiagram> unique_diagrams;
    for (const Gp7::Track &track : doc.myTracks)
    {
        for (auto &&[_, chord] : track.myChords)
            unique_diagrams.insert(convertChordDiagram(chord));
    }

    // Sort by chord name to insert in a deterministic order.
    std::vector<ChordDiagram> diagrams(unique_diagrams.begin(),
                                       unique_diagrams.end());
    std::sort(diagrams.begin(), diagrams.end(),
              [](const ChordDiagram &d1, const ChordDiagram &d2)
              { return Util::toString(d1) < Util::toString(d2); });

    for (const ChordDiagram &diagram : diagrams)
        score.insertChordDiagram(diagram);
}

static void
convertSystem(const Gp7::Document &doc, Score &score, int bar_begin,
              int bar_end)
{
    System system;

    // For the first system, create the players and assign them to the staves.
    if (bar_begin == 0)
    {
        PlayerChange initial_player_change;
        convertPlayers(doc.myTracks, score, initial_player_change);
        system.insertPlayerChange(initial_player_change);
    }

    // Create a staff for each player.
    for (auto &&player : score.getPlayers())
        system.insertStaff(Staff(player.getTuning().getStringCount()));

    int start_pos = 0;
    int system_bar_idx = 0;
    for (int bar_idx = bar_begin; bar_idx < bar_end; ++bar_idx)
    {
        const int num_staves = static_cast<int>(score.getPlayers().size());
        const Gp7::MasterBar &master_bar = doc.myMasterBars.at(bar_idx);

        // If the previous bar was a repeat end, and this master bar is a
        // repeat start, we'll need to create a separate adjacent barline.
        if (master_bar.myRepeatStart &&
            system.getBarlines()[system_bar_idx].getBarType() ==
                Barline::RepeatEnd)
        {
            Barline barline;
            barline.setPosition(start_pos);
            system.insertBarline(barline);
            ++system_bar_idx;
            ++start_pos;
        }

        // Go through the bar for each staff.
        int end_pos = start_pos;
        for (int staff_idx = 0; staff_idx < num_staves; ++staff_idx)
        {
            Staff &staff = system.getStaves()[staff_idx];
            const Player &player = score.getPlayers()[staff_idx];
            const Tuning &tuning = player.getTuning();

            const int gp_bar_idx = master_bar.myBarIds[staff_idx];
            if (gp_bar_idx < 0)
                continue; // Empty bar
            const Gp7::Bar &gp_bar = doc.myBars.at(gp_bar_idx);

            // For the first bar in the system, set the staff's clefy type.
            if (bar_idx == bar_begin)
                staff.setClefType(convertClefType(gp_bar.myClefType));

            const int num_voices =
                std::min<int>(Staff::NUM_VOICES, gp_bar.myVoiceIds.size());
            for (int voice_idx = 0; voice_idx < num_voices; ++voice_idx)
            {
                const int gp_voice_idx = gp_bar.myVoiceIds[voice_idx];
                // Voice might not be used.
                if (gp_voice_idx < 0)
                    continue;

                const Gp7::Voice &gp_voice = doc.myVoices.at(gp_voice_idx);
                Voice &voice = staff.getVoices()[voice_idx];

                int voice_pos = start_pos;
                boost::rational<int> time;
                for (int gp_beat_idx : gp_voice.myBeatIds)
                {
                    const Gp7::Beat &gp_beat = doc.myBeats.at(gp_beat_idx);
                    const Gp7::Rhythm &gp_rhythm =
                        doc.myRhythms.at(gp_beat.myRhythmId);

                    // Create a text item in the system if necessary.
                    if (!gp_beat.myFreeText.empty())
                    {
                        system.insertTextItem(
                            TextItem(voice_pos, gp_beat.myFreeText));
                    }

                    if (gp_beat.myChordId)
                    {
                        // FIXME - doesn't handle dual-staff tracks.
                        const int track_idx = staff_idx;

                        const Gp7::Chord &gp_chord =
                            doc.myTracks[track_idx].myChords.at(*gp_beat.myChordId);

                        ChordName chord_name = convertChordName(gp_chord.myName);
                        // Avoid inserting duplicates since many tracks may
                        // have the same chord name.
                        if (!ScoreUtils::findByPosition(system.getChords(),
                                                        voice_pos))
                        {
                            system.insertChord(
                                ChordText(voice_pos, chord_name));
                        }
                    }

                    Position pos = convertPosition(gp_beat, gp_rhythm);
                    pos.setPosition(voice_pos++);

                    if (master_bar.myFermatas.count(time))
                        pos.setProperty(Position::Fermata);

                    // Flag as a rest if there are no notes.
                    if (gp_beat.myNoteIds.empty())
                        pos.setRest();

                    // Import notes.
                    for (int gp_note_id : gp_beat.myNoteIds)
                    {
                        const Gp7::Note &gp_note = doc.myNotes.at(gp_note_id);

                        Note note = convertNote(pos, gp_beat, gp_note, tuning);
                        if (Utils::findByString(pos, note.getString()))
                        {
                            // This happens for drums in .gpx files, which
                            // don't specify a string / fret number.
#if 0
                            std::cerr << "Colliding notes at string "
                                      << note.getString() << ", staff "
                                      << staff_idx << std::endl;
#endif
                        }
                        else
                            pos.insertNote(std::move(note));
                    }

                    // Take irregular groups into account when computing the
                    // duration time (the groups aren't constructed until all
                    // notes are created).
                    boost::rational<int> duration =
                        VoiceUtils::getDurationTime(voice, pos);
                    if (gp_rhythm.myTupletNum)
                    {
                        duration *= boost::rational<int>(
                            gp_rhythm.myTupletDenom, gp_rhythm.myTupletNum);
                    }

                    time += duration;

                    voice.insertPosition(std::move(pos));
                }

                convertIrregularGroupings(voice, start_pos, voice_pos, doc,
                                          gp_voice);

                end_pos = std::max(voice_pos, end_pos);
            }
        }

        // Get the surrounding barlines and set their properties.
        const bool final_bar = size_t(bar_idx + 1) == doc.myMasterBars.size();
        Barline &bar_1 = system.getBarlines()[system_bar_idx];
        Barline bar_2;
        bar_2.setPosition(end_pos);
        const Gp7::MasterBar *prev_master_bar =
            (bar_idx != 0) ? &doc.myMasterBars[bar_idx - 1] : nullptr;
        convertBarline(bar_1, bar_2, master_bar, prev_master_bar, final_bar);

        convertTempoMarkers(system, bar_1.getPosition(), master_bar);
        convertAlternateEndings(system, bar_1.getPosition(), master_bar);
        convertDirections(system, start_pos, end_pos, master_bar);

        // Insert a new barline unless we're finishing the system, in which
        // case we just need to modify the end bar.
        if (bar_idx != (bar_end - 1))
            system.insertBarline(bar_2);
        else
            system.getBarlines().back() = bar_2;

        ++system_bar_idx;
        start_pos = end_pos + 1;
    }

    score.insertSystem(std::move(system));
}

static bool
isValidLayout(const std::vector<int> &layout, int num_bars)
{
    // The layout's number of bars may not be an exact match - the final system
    // in the layout might include room for more bars.
    return std::accumulate(layout.begin(), layout.end(), 0) >= num_bars;
}

void
Gp7::convert(const Gp7::Document &doc, Score &score)
{
    convertScoreInfo(doc.myScoreInfo, score);

    // The multi-track layout is sometimes invalid (particularly for .gpx
    // files). So, fall back to the first track's layout if we need to.
    std::vector<int> layout = doc.myScoreInfo.myScoreSystemsLayout;
    if (!isValidLayout(layout, static_cast<int>(doc.myMasterBars.size())) &&
        !doc.myTracks.empty())
    {
        layout = doc.myTracks[0].mySystemsLayout;
        assert(isValidLayout(layout, static_cast<int>(doc.myMasterBars.size())));
    }

    int bar_idx = 0;
    for (int num_bars : layout)
    {
        const int bar_end = std::min(bar_idx + num_bars,
                                     static_cast<int>(doc.myMasterBars.size()));
        convertSystem(doc, score, bar_idx, bar_end);
        bar_idx += num_bars;
    }

    convertChordDiagrams(doc, score);

    ScoreUtils::adjustRehearsalSigns(score);
    ScoreUtils::polishScore(score);
    ScoreUtils::addStandardFilters(score);
}
