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

#include "converter.h"
#include "parser.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/rational.hpp>

#include <formats/fileformat.h>
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
#include <score/voiceutils.h>

#include <iostream>

/// Convert the Guitar Pro file metadata.
static void
convertScoreInfo(const Gp7::ScoreInfo &gp_info, Score &score)
{
    ScoreInfo info;
    SongData data;

    // No support for subtitle.
    // TODO - consider adding this to SongData.
    data.setTitle(gp_info.myTitle);
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
        const int instrument_idx = score.getInstruments().size();

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
            tuning.setNotes(std::vector<uint8_t>(staff.myTuning.rbegin(),
                                                 staff.myTuning.rend()));
            tuning.setCapo(staff.myCapo);
            player.setTuning(tuning);

            const int player_idx = score.getPlayers().size();
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

static Note
convertNote(Position &position, const Gp7::Beat &gp_beat,
            const Gp7::Note &gp_note, const Tuning &tuning)
{
    using HarmonicType = Gp7::Note::HarmonicType;

    Note note;
    note.setFretNumber(gp_note.myFret);
    // String numbers are flipped around.
    note.setString(tuning.getStringCount() - gp_note.myString - 1);

    note.setProperty(Note::Tied, gp_note.myTied);
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
    if (gp_note.myAccentTypes.test(int(GpAccentType::Accent)))
        position.setProperty(Position::Sforzando);
    if (gp_note.myAccentTypes.test(int(GpAccentType::HeavyAccent)))
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
            note.setTappedHarmonicFret(note.getFretNumber() +
                                       gp_note.myHarmonicFret);
        }

        // TODO - import artificial harmonics
    }

    // The GP7 trill stores the note value, not the fret number.
    if (gp_note.myTrillNote)
    {
        note.setTrilledFret(*gp_note.myTrillNote -
                            tuning.getNote(note.getString(), false));
    }

    if (gp_note.myLeftFinger)
    {
        using FingerType = Gp7::Note::FingerType;
        switch (*gp_note.myLeftFinger)
        {
            case FingerType::Open:
                note.setLeftHandFingering(
                    LeftHandFingering(LeftHandFingering::None));
                break;
            case FingerType::C:
                note.setLeftHandFingering(
                    LeftHandFingering(LeftHandFingering::Little));
                break;
            case FingerType::A:
                note.setLeftHandFingering(
                    LeftHandFingering(LeftHandFingering::Ring));
                break;
            case FingerType::M:
                note.setLeftHandFingering(
                    LeftHandFingering(LeftHandFingering::Middle));
                break;
            case FingerType::I:
                note.setLeftHandFingering(
                    LeftHandFingering(LeftHandFingering::Index));
                break;
            case FingerType::P:
                // TODO - thumb is not currently support for fingerings.
                break;
        }
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

    return pos;
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
        const int num_staves = score.getPlayers().size();
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
        for (int staff_idx = 0; staff_idx < num_staves ; ++staff_idx)
        {
            Staff &staff = system.getStaves()[staff_idx];
            const Player &player = score.getPlayers()[staff_idx];
            const Tuning &tuning = player.getTuning();

            const int gp_bar_idx = master_bar.myBarIds[staff_idx];
            const Gp7::Bar &gp_bar = doc.myBars.at(gp_bar_idx);

            // For the first bar in the system, set the staff's clefy type.
            if (bar_idx == bar_begin)
                staff.setClefType(convertClefType(gp_bar.myClefType));

            assert(gp_bar.myVoiceIds.size() == 4);
            for (int voice_idx = 0; voice_idx < Staff::NUM_VOICES; ++voice_idx)
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

                    Position pos = convertPosition(gp_beat, gp_rhythm);
                    pos.setPosition(voice_pos++);

                    // TODO - convert irregular groupings.
                    // Note that for fermatas, irregular groups must also be
                    // taken into account before computing the beat's time.

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
                            throw FileFormatException("Colliding notes!");
                        else
                            pos.insertNote(note);
                    }

                    voice.insertPosition(pos);
                    time += VoiceUtils::getDurationTime(voice, pos);
                }

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

        // Insert a new barline unless we're finishing the system, in which
        // case we just need to modify the end bar.
        if (bar_idx != (bar_end - 1))
            system.insertBarline(bar_2);
        else
            system.getBarlines().back() = bar_2;

        ++system_bar_idx;
        start_pos = end_pos + 1;
    }

    score.insertSystem(system);
}

void
Gp7::convert(const Gp7::Document &doc, Score &score)
{
    convertScoreInfo(doc.myScoreInfo, score);

    // If there is only one track, follow its layout instead of the multi-track
    // layout.
    std::vector<int> layout = doc.myScoreInfo.myScoreSystemsLayout;
    if (doc.myTracks.size() == 1)
        layout = doc.myTracks[0].mySystemsLayout;

    int bar_idx = 0;
    for (int num_bars : layout)
    {
        const int bar_end = std::min(bar_idx + num_bars,
                                     static_cast<int>(doc.myMasterBars.size()));
        convertSystem(doc, score, bar_idx, bar_end);
        bar_idx += num_bars;
    }

    ScoreUtils::adjustRehearsalSigns(score);
}
