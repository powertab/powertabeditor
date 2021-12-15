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

#include "gp345to7converter.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <cmath>

static Gp7::ScoreInfo
convertScoreInfo(const Gp::Document &doc)
{
    const Gp::Header &header = doc.myHeader;

    Gp7::ScoreInfo gp7_info;
    gp7_info.myTitle = header.myTitle;
    gp7_info.mySubtitle = header.mySubtitle;
    gp7_info.myArtist = header.myArtist;
    gp7_info.myAlbum = header.myAlbum;
    gp7_info.myWords = header.myLyricist;
    gp7_info.myMusic = header.myComposer;
    gp7_info.myCopyright = header.myCopyright;
    gp7_info.myTabber = header.myTranscriber;
    gp7_info.myInstructions = header.myInstructions;
    gp7_info.myNotices = boost::algorithm::join(header.myNotices, "\n");

    // Decide how the measures are split into systems.
    // For now just do three measures per system.
    static constexpr int measures_per_system = 3;
    const int num_measures = static_cast<int>(doc.myMeasures.size());
    const int num_systems = num_measures / measures_per_system;
    const int remainder =  num_measures % measures_per_system;
    std::vector<int> &layout = gp7_info.myScoreSystemsLayout;

    layout.insert(layout.begin(), num_systems, measures_per_system);
    if (remainder != 0)
        layout.push_back(remainder);

    // TODO - convert lyrics.

    return gp7_info;
}

static std::vector<Gp7::Track>
convertTracks(const Gp::Document &doc)
{
    std::vector<Gp7::Track> gp7_tracks;

    for (const Gp::Track &track : doc.myTracks)
    {
        Gp7::Track gp7_track;
        gp7_track.myName = track.myName;
        // Not setting mySystemsLayout, since there is no GP5 equivalent. We
        // already set the score-wide system layout in the score info.

        Gp7::Staff staff;
        staff.myCapo = track.myCapo;
        for (uint8_t note : track.myTuning)
            staff.myTuning.push_back(note);
        // The string numbers are flipped around in GP7.
        std::reverse(staff.myTuning.begin(), staff.myTuning.end());

        gp7_track.myStaves = { staff };

        const Gp::Channel &channel = doc.myChannels.at(track.myChannelIndex);
        Gp7::Sound sound;
        sound.myLabel = track.myName;
        sound.myMidiPreset = channel.myInstrument;
        gp7_track.mySounds = { sound };

        gp7_tracks.push_back(gp7_track);
    }

    return gp7_tracks;
}

/// String numbers are reversed in GP7.
static int
convertStringNumber(const Gp::Track &track, int string)
{
    return static_cast<int>(track.myTuning.size()) - string - 1;
}

static std::optional<Gp7::Beat>
convertGraceNotes(const Gp::Beat &beat, const Gp::Track &track,
                  Gp7::Document &gp7_doc)
{
    for (const Gp::Note &note : beat.myNotes)
    {
        if (!note.myGraceNote)
            continue;

        Gp7::Beat gp7_beat;
        Gp7::Note gp7_note;
        gp7_beat.myGraceNote = true;
        gp7_note.myString = convertStringNumber(track, note.myString);
        gp7_note.myFret = note.myGraceNote->myFret;

        switch (note.myGraceNote->myTransition)
        {
            case Gp::GraceNote::HammerTransition:
                gp7_note.myHammerOn = true;
                break;
            case Gp::GraceNote::SlideTransition:
                gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::Shift));
                break;
            default:
                break;
        }

        Gp7::Rhythm rhythm;
        rhythm.myDuration = note.myGraceNote->myDuration;

        gp7_doc.addRhythm(gp7_beat, std::move(rhythm));
        gp7_doc.addNote(gp7_beat, std::move(gp7_note));
        return gp7_beat;
    }

    return {};
}

/// Finds the last note in the provided voice that occurs on the specified
/// string.
static Gp7::Note *
findTiedNoteOrigin(Gp7::Document &doc, const Gp7::Voice &voice,
                   const int string)
{
    using namespace boost::adaptors;

    for (int beat_id : reverse(voice.myBeatIds))
    {
        const Gp7::Beat &beat = doc.myBeats.at(beat_id);
        for (int note_id : beat.myNoteIds)
        {
            Gp7::Note &note = doc.myNotes.at(note_id);
            if (note.myString == string)
                return &note;
        }
    }

    return nullptr;
}

/// Searches through the current and previous measures of this voice for the
/// origin tied note.
static Gp7::Note *
findTiedNoteOrigin(Gp7::Document &doc, const Gp7::Voice &current_voice,
                   const int staff_idx, const int voice_idx, const int string)
{
    using namespace boost::adaptors;

    // First, check the current measure that is tn the process of being built.
    Gp7::Note *tie_origin = findTiedNoteOrigin(doc, current_voice, string);
    if (tie_origin)
        return tie_origin;

    // Otherwise, search through all the previous measures.
    for (const Gp7::MasterBar &master_bar : reverse(doc.myMasterBars))
    {
        const int bar_id = master_bar.myBarIds.at(staff_idx);
        const Gp7::Bar &bar = doc.myBars.at(bar_id);
        const Gp7::Voice &voice = doc.myVoices.at(bar.myVoiceIds[voice_idx]);

        tie_origin = findTiedNoteOrigin(doc, voice, string);
        if (tie_origin)
            return tie_origin;
    }

    return nullptr;
}

static Gp7::Bend
convertBend(const Gp::Bend &bend)
{
    using Point = Gp::Bend::Point;

    Gp7::Bend gp7_bend;
    const std::vector<Point> &points = bend.myPoints;
    assert(points.size() >= 2);

    // Default output: just use the first and last points as the origin / dest.
    gp7_bend.myOriginValue = points.front().myValue;
    gp7_bend.myOriginOffset = points.front().myOffset;
    gp7_bend.myDestValue = points.back().myValue;
    gp7_bend.myDestOffset = points.back().myOffset;

    // Try to find a middle region.
    const int start_value = points.front().myValue;
    [[maybe_unused]] const int end_value = points.back().myValue;
    auto middle_1 =
        std::find_if(points.begin(), points.end(), [&](const Point &point) {
            return point.myValue != start_value;
        });

    if (middle_1 != points.end())
    {
        const int middle_value = middle_1->myValue;
        auto dest =
            std::find_if(middle_1, points.end(), [&](const Point &point) {
                return point.myValue != middle_value;
            });

        if (dest != points.end())
        {
            auto middle_2 = std::prev(dest);

            gp7_bend.myMiddleOffset1 = middle_1->myOffset;
            gp7_bend.myMiddleOffset2 = middle_2->myOffset;
            gp7_bend.myMiddleValue = middle_value;

            auto origin = std::prev(middle_1);
            assert(origin->myValue == start_value);
            gp7_bend.myOriginOffset = origin->myOffset;
            assert(dest->myValue == end_value);
            gp7_bend.myDestOffset = dest->myOffset;
        }
    }
    else
    {
        // Otherwise, fill in a dummy middle region. This makes it simpler to
        // detect whammy bar dips.
        gp7_bend.myMiddleOffset1 = gp7_bend.myMiddleOffset2 =
            0.5 * (gp7_bend.myOriginOffset + gp7_bend.myDestOffset);
        gp7_bend.myMiddleValue =
            0.5 * (gp7_bend.myOriginValue + gp7_bend.myDestValue);
    }

    return gp7_bend;
}

static void
convertBeat(const Gp::Beat &beat, const Gp::Track &track,
            Gp7::Document &gp7_doc, const Gp7::Voice &gp7_current_voice,
            const int staff_idx, const int voice_idx, Gp7::Beat &gp7_beat)
{
    // Convert the rhythm.
    {
        Gp7::Rhythm rhythm;
        rhythm.myDuration = beat.myDuration;
        if (beat.myIsDotted)
            rhythm.myDots = 1;

        if (beat.myIrregularGrouping)
        {
            rhythm.myTupletNum = *beat.myIrregularGrouping;
            // The denominator of the irregular grouping is the nearest
            // power of 2 (from below).
            rhythm.myTupletDenom = static_cast<int>(std::pow(
                2, std::floor(std::log(rhythm.myTupletNum) / std::log(2.0))));
        }

        gp7_doc.addRhythm(gp7_beat, std::move(rhythm));
    }

    if (beat.myOctave8va)
        gp7_beat.myOttavia = Gp7::Beat::Ottavia::O8va;
    else if (beat.myOctave8vb)
        gp7_beat.myOttavia = Gp7::Beat::Ottavia::O8vb;
    else if (beat.myOctave15ma)
        gp7_beat.myOttavia = Gp7::Beat::Ottavia::O15ma;
    else if (beat.myOctave15mb)
        gp7_beat.myOttavia = Gp7::Beat::Ottavia::O15mb;

    gp7_beat.myTremoloPicking = beat.myIsTremoloPicked;
    gp7_beat.myBrushUp = beat.myPickstrokeUp;
    gp7_beat.myBrushDown = beat.myPickstrokeDown;
    if (beat.myText)
        gp7_beat.myFreeText = *beat.myText;

    if (beat.myTremoloBar)
        gp7_beat.myWhammy = convertBend(*beat.myTremoloBar);

    for (const Gp::Note &note : beat.myNotes)
    {
        Gp7::Note gp7_note;

        // The string numbers are reversed in GP7.
        gp7_note.myString = convertStringNumber(track, note.myString);
        gp7_note.myFret = note.myFret;

        if (note.myIsTied)
        {
            // Tied notes inherit their fret from the previous note, and don't
            // always have a correct fret number stored in the GP3/4/5 file.
            Gp7::Note *prev_note =
                findTiedNoteOrigin(gp7_doc, gp7_current_voice, staff_idx,
                                   voice_idx, gp7_note.myString);
            assert(prev_note);
            if (prev_note)
            {
                gp7_note.myFret = prev_note->myFret;
                gp7_note.myTieDest = true;
                prev_note->myTieOrigin = true;
            }
        }

        gp7_note.myPalmMuted = note.myHasPalmMute;
        gp7_note.myMuted = note.myIsMuted;
        gp7_note.myGhost = note.myIsGhostNote;
        gp7_note.myTapped = beat.myIsTapped;
        gp7_note.myHammerOn = note.myIsHammerOnOrPullOff;
        gp7_note.myVibrato = beat.myIsVibrato || note.myIsVibrato;
        gp7_note.myWideVibrato = beat.myIsWideVibrato;
        gp7_note.myLetRing = note.myIsLetRing;

        if (note.myIsStaccato)
            gp7_note.myAccentTypes.set(int(Gp7::Note::AccentType::Staccato));

        if (note.myHasHeavyAccent)
            gp7_note.myAccentTypes.set(int(Gp7::Note::AccentType::HeavyAccent));
        else if (note.myHasAccent)
            gp7_note.myAccentTypes.set(int(Gp7::Note::AccentType::Accent));

        if (beat.myIsNaturalHarmonic || note.myIsNaturalHarmonic)
            gp7_note.myHarmonic = Gp7::Note::HarmonicType::Natural;

        if (beat.myIsArtificialHarmonic)
        {
            // GP3 artificial harmonics don't specify a pitch.
            gp7_note.myHarmonic = Gp7::Note::HarmonicType::Artificial;
            gp7_note.myHarmonicFret = 12;
        }
        else if (note.myIsArtificialHarmonic)
        {
            gp7_note.myHarmonic = Gp7::Note::HarmonicType::Artificial;
            gp7_note.myHarmonicFret = note.myHarmonicFret;
        }

        if (note.myIsTappedHarmonic)
        {
            gp7_note.myHarmonic = Gp7::Note::HarmonicType::Tap;
            gp7_note.myHarmonicFret = note.myHarmonicFret;
        }

        if (note.myIsShiftSlide)
            gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::Shift));
        if (note.myIsLegatoSlide)
            gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::Legato));
        if (note.myIsSlideInAbove)
            gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::SlideInAbove));
        if (note.myIsSlideInBelow)
            gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::SlideInBelow));
        if (note.myIsSlideOutUp)
            gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::SlideOutUp));
        if (note.myIsSlideOutDown)
            gp7_note.mySlideTypes.set(int(Gp7::Note::SlideType::SlideOutDown));

        if (note.myTrilledFret)
        {
            // In GP7, the MIDI note value is stored, not the fret number.
            gp7_note.myTrillNote =
                *note.myTrilledFret + track.myTuning[note.myString];
        }

        if (note.myLeftFinger)
        {
            switch (*note.myLeftFinger)
            {
                case 1:
                    gp7_note.myLeftFinger = Gp7::Note::FingerType::I;
                    break;
                case 2:
                    gp7_note.myLeftFinger = Gp7::Note::FingerType::M;
                    break;
                case 3:
                    gp7_note.myLeftFinger = Gp7::Note::FingerType::A;
                    break;
                case 4:
                    gp7_note.myLeftFinger = Gp7::Note::FingerType::C;
                    break;
                case 0:
                    gp7_note.myLeftFinger = Gp7::Note::FingerType::P;
                    break;
                case -1:
                default:
                    gp7_note.myLeftFinger = Gp7::Note::FingerType::Open;
                    break;
            }
        }

        if (note.myBend)
            gp7_note.myBend = convertBend(*note.myBend);

        if (note.myIsTremoloPicked)
            gp7_beat.myTremoloPicking = true;

        gp7_doc.addNote(gp7_beat, std::move(gp7_note));
    }
}

static void
convertMasterBars(const Gp::Document &doc, Gp7::Document &gp7_doc)
{
    for (const Gp::Measure &measure : doc.myMeasures)
    {
        Gp7::MasterBar master_bar;
        master_bar.myDoubleBar = measure.myIsDoubleBar;
        master_bar.myRepeatStart = measure.myIsRepeatBegin;
        master_bar.myRepeatEnd = measure.myRepeatEnd.has_value();
        if (master_bar.myRepeatEnd)
            master_bar.myRepeatCount = *measure.myRepeatEnd;

        if (measure.myMarker)
        {
            Gp7::MasterBar::Section section;
            section.myText = *measure.myMarker;
            master_bar.mySection = section;
        }

        // Copy key signature from the previous bar if there isn't a key change.
        const Gp7::MasterBar *prev_master_bar =
            !gp7_doc.myMasterBars.empty() ? &gp7_doc.myMasterBars.back()
                                          : nullptr;

        if (measure.myKeyChange)
        {
            const int accidentals = measure.myKeyChange->myAccidentals;
            master_bar.myKeySig.myAccidentalCount = std::abs(accidentals);
            master_bar.myKeySig.mySharps = accidentals >= 0;
            master_bar.myKeySig.myMinor = measure.myKeyChange->myIsMinor;
        }
        else if (prev_master_bar)
            master_bar.myKeySig = prev_master_bar->myKeySig;
        else
        {
            master_bar.myKeySig.myAccidentalCount = std::abs(doc.myInitialKey);
            master_bar.myKeySig.mySharps = doc.myInitialKey >= 0;
        }

        if (measure.myTimeSignatureChange)
        {
            master_bar.myTimeSig.myBeats =
                measure.myTimeSignatureChange->myNumerator;
            master_bar.myTimeSig.myBeatValue =
                measure.myTimeSignatureChange->myDenominator;
        }
        else if (prev_master_bar)
            master_bar.myTimeSig = prev_master_bar->myTimeSig;

        // Initial tempo. Later tempo changes are stored on the beats.
        if (gp7_doc.myMasterBars.empty())
        {
            Gp7::TempoChange tempo_change;
            tempo_change.myBeatsPerMinute = doc.myStartTempo;
            tempo_change.myDescription = doc.myStartTempoName;
            tempo_change.myIsVisible = doc.myStartTempoVisible;
            master_bar.myTempoChanges.push_back(tempo_change);
        }

        if (measure.myAlternateEnding)
        {
            // Each bit represents an alternate ending from 1 to 8.
            static constexpr int num_bits = 8;
            std::bitset<num_bits> bits(*measure.myAlternateEnding);
            for (int i = 0; i < num_bits; ++i)
            {
                if (bits.test(i))
                    master_bar.myAlternateEndings.push_back(i + 1);
            }
        }

        // Import the bars for each track.
        for (size_t staff_idx = 0; staff_idx < measure.myStaves.size();
             ++staff_idx)
        {
            const Gp::Track &track = doc.myTracks[staff_idx];
            const Gp::Staff &staff = measure.myStaves[staff_idx];

            Gp7::Bar bar;

            for (size_t voice_idx = 0; voice_idx < staff.myVoices.size();
                 ++voice_idx)
            {
                const std::vector<Gp::Beat> &voice = staff.myVoices[voice_idx];
                Gp7::Voice gp7_voice;

                for (const Gp::Beat &beat : voice)
                {
                    if (beat.myIsEmpty)
                        continue;

                    auto grace_beat = convertGraceNotes(beat, track, gp7_doc);
                    if (grace_beat)
                        gp7_doc.addBeat(gp7_voice, std::move(*grace_beat));

                    Gp7::Beat gp7_beat;
                    convertBeat(beat, track, gp7_doc, gp7_voice,
                                static_cast<int>(staff_idx),
                                static_cast<int>(voice_idx), gp7_beat);
                    gp7_doc.addBeat(gp7_voice, std::move(gp7_beat));

                    // Add tempo changes to the measure.
                    // TODO - set the position fraction for tempo changes that
                    // occur during the bar. When this is supported by the GP7
                    // converter, enable the unit tests for this.
                    if (beat.myTempoChange)
                    {
                        Gp7::TempoChange tempo_change;
                        tempo_change.myBeatsPerMinute = *beat.myTempoChange;
                        tempo_change.myDescription = beat.myTempoChangeName;
                        master_bar.myTempoChanges.push_back(tempo_change);
                    }
                }

                gp7_doc.addVoice(bar, std::move(gp7_voice));
            }

            // GP7 expects there to be 4 voices.
            for (size_t i = bar.myVoiceIds.size(); i < 4; ++i)
                bar.myVoiceIds.push_back(-1);

            gp7_doc.addBar(master_bar, std::move(bar));
        }

        gp7_doc.myMasterBars.push_back(master_bar);
    }
}

static void
convertDirections(const Gp::DirectionMap &dirs, Gp7::Document &gp7_doc)
{
    using Target = Gp7::MasterBar::DirectionTarget;
    auto convertTarget = [&](int index, Target target) {
        if (index >= 0)
            gp7_doc.myMasterBars[index].myDirectionTargets.push_back(target);
    };

    convertTarget(dirs.myCoda, Target::Coda);
    convertTarget(dirs.myDoubleCoda, Target::DoubleCoda);
    convertTarget(dirs.mySegno, Target::Segno);
    convertTarget(dirs.mySegnoSegno, Target::SegnoSegno);
    convertTarget(dirs.myFine, Target::Fine);

    using Jump = Gp7::MasterBar::DirectionJump;
    auto convertJump = [&](int index, Jump jump) {
        if (index >= 0)
            gp7_doc.myMasterBars[index].myDirectionJumps.push_back(jump);
    };

    convertJump(dirs.myDaCapo, Jump::DaCapo);
    convertJump(dirs.myDaCapoAlCoda, Jump::DaCapoAlCoda);
    convertJump(dirs.myDaCapoAlDoubleCoda, Jump::DaCapoAlDoubleCoda);
    convertJump(dirs.myDaCapoAlFine, Jump::DaCapoAlFine);
    convertJump(dirs.myDaSegno, Jump::DaSegno);
    convertJump(dirs.myDaSegnoAlCoda, Jump::DaSegnoAlCoda);
    convertJump(dirs.myDaSegnoAlDoubleCoda, Jump::DaSegnoAlDoubleCoda);
    convertJump(dirs.myDaSegnoAlFine, Jump::DaSegnoAlFine);
    convertJump(dirs.myDaSegnoSegno, Jump::DaSegnoSegno);
    convertJump(dirs.myDaSegnoSegnoAlCoda, Jump::DaSegnoSegnoAlCoda);
    convertJump(dirs.myDaSegnoSegnoAlDoubleCoda,
                Jump::DaSegnoSegnoAlDoubleCoda);
    convertJump(dirs.myDaSegnoSegnoAlFine, Jump::DaSegnoSegnoAlFine);
    convertJump(dirs.myDaCoda, Jump::DaCoda);
    convertJump(dirs.myDaDoubleCoda, Jump::DaDoubleCoda);
}

Gp7::Document
Gp::convertToGp7(const Gp::Document &doc)
{
    Gp7::Document gp7_doc;
    gp7_doc.myScoreInfo = convertScoreInfo(doc);
    gp7_doc.myTracks = convertTracks(doc);
    convertMasterBars(doc, gp7_doc);
    convertDirections(doc.myDirections, gp7_doc);
    return gp7_doc;
}
