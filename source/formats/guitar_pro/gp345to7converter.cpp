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
    const int num_systems = doc.myMeasures.size() / measures_per_system;
    const int remainder =  doc.myMeasures.size() % measures_per_system;
    std::vector<int> &layout = gp7_info.myScoreSystemsLayout;

    layout.insert(layout.begin(), num_systems, measures_per_system);
    if (remainder != 0)
        layout.push_back(remainder);

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
    return track.myTuning.size() - string - 1;
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

static void
convertBeat(const Gp::Beat &beat, const Gp::Track &track,
            Gp7::Document &gp7_doc, Gp7::Beat &gp7_beat)
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
            rhythm.myTupletDenom = std::pow(
                2, std::floor(std::log(rhythm.myTupletNum) / std::log(2.0)));
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

    for (const Gp::Note &note : beat.myNotes)
    {
        Gp7::Note gp7_note;

        // The string numbers are reversed in GP7.
        gp7_note.myString = convertStringNumber(track, note.myString);
        gp7_note.myFret = note.myFret;
        gp7_note.myPalmMuted = note.myHasPalmMute;
        gp7_note.myMuted = note.myIsMuted;
        // TODO - tied notes need to inherit their fret from the previous note.
        gp7_note.myTieDest = note.myIsTied;
        gp7_note.myGhost = note.myIsGhostNote;
        gp7_note.myTapped = beat.myIsTapped;
        gp7_note.myHammerOn = note.myIsHammerOnOrPullOff;
        // TODO - is wide vibrato stored in GP3/4/5?
        gp7_note.myVibrato = beat.myIsVibrato || note.myIsVibrato;
        gp7_note.myLetRing = note.myIsLetRing;

        if (note.myIsStaccato)
            gp7_note.myAccentTypes.set(int(Gp7::Note::AccentType::Staccato));
        if (note.myHasAccent)
            gp7_note.myAccentTypes.set(int(Gp7::Note::AccentType::Accent));
        if (note.myHasHeavyAccent)
            gp7_note.myAccentTypes.set(int(Gp7::Note::AccentType::HeavyAccent));

        if (beat.myIsNaturalHarmonic || note.myIsNaturalHarmonic)
            gp7_note.myHarmonic = Gp7::Note::HarmonicType::Natural;
        if (beat.myIsArtificialHarmonic)
        {
            gp7_note.myHarmonic = Gp7::Note::HarmonicType::Artificial;
            // FIXME - import the harmonic fret properly from GP3/4/5 files!
            gp7_note.myHarmonicFret = 24;
        }
        // TODO - tapped harmonics are not yet implemented for GP3/4/5

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

        // TODO - implement left hand fingering.
        // TODO - implement bends.  We might also need to set up myTieOrigin
        // for this to import held bends properly.

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

        // TODO - import time / key signatures.
        // TODO - import directions.
        // TODO - import tempo changes.

        if (measure.myAlternateEnding)
            master_bar.myAlternateEndings.push_back(*measure.myAlternateEnding);

        // Import the bars for each track.
        for (size_t staff_idx = 0; staff_idx < measure.myStaves.size();
             ++staff_idx)
        {
            const Gp::Track &track = doc.myTracks[staff_idx];
            const Gp::Staff &staff = measure.myStaves[staff_idx];

            Gp7::Bar bar;

            for (const std::vector<Gp::Beat> &voice : staff.myVoices)
            {
                Gp7::Voice gp7_voice;

                for (const Gp::Beat &beat : voice)
                {
                    if (beat.myIsEmpty)
                        continue;

                    auto grace_beat = convertGraceNotes(beat, track, gp7_doc);
                    if (grace_beat)
                        gp7_doc.addBeat(gp7_voice, std::move(*grace_beat));

                    Gp7::Beat gp7_beat;
                    convertBeat(beat, track, gp7_doc, gp7_beat);
                    gp7_doc.addBeat(gp7_voice, std::move(gp7_beat));
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

Gp7::Document
Gp::convertToGp7(const Gp::Document &doc)
{
    Gp7::Document gp7_doc;
    gp7_doc.myScoreInfo = convertScoreInfo(doc);
    gp7_doc.myTracks = convertTracks(doc);
    convertMasterBars(doc, gp7_doc);
    return gp7_doc;
}
