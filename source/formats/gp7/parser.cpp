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

#include "parser.h"

#include <formats/fileformat.h>

#include <boost/algorithm/string/split.hpp>
#include <string>

#include <iostream>

bool
Gp7::MasterBar::TimeSignature::operator==(const TimeSignature &other) const
{
    return myBeats == other.myBeats && myBeatValue == other.myBeatValue;
}

bool
Gp7::MasterBar::TimeSignature::operator!=(const TimeSignature &other) const
{
    return !operator==(other);
}

bool
Gp7::MasterBar::KeySignature::operator==(const KeySignature &other) const
{
    return myAccidentalCount == other.myAccidentalCount &&
           myMinor == other.myMinor && mySharps == other.mySharps;
}

bool
Gp7::MasterBar::KeySignature::operator!=(const KeySignature &other) const
{
    return !operator==(other);
}

static std::vector<std::string>
splitString(std::string input, char separator = ' ')
{
    if (input.empty())
        return {};

    std::vector<std::string> output;
    boost::algorithm::split(output, input,
                            [=](char c) { return c == separator; });
    return output;
}

static std::vector<int>
toIntList(const std::vector<std::string> &input)
{
    std::vector<int> output;
    output.reserve(input.size());

    for (auto &&val : input)
        output.push_back(std::stoi(val));

    return output;
}

static Gp7::ScoreInfo
parseScoreInfo(const pugi::xml_node &node)
{
    Gp7::ScoreInfo info;
    info.myTitle = node.child_value("Title");
    info.mySubtitle = node.child_value("SubTitle");
    info.myArtist = node.child_value("Artist");
    info.myAlbum = node.child_value("Album");
    // Skipping the 'WordsAndMusic' node, which seems to be unused.
    info.myWords = node.child_value("Words");
    info.myMusic = node.child_value("Music");
    info.myCopyright = node.child_value("Copyright");
    info.myTabber = node.child_value("Tabber");
    info.myInstructions = node.child_value("Instructions");
    info.myNotices = node.child_value("Notices");

    // Skipping FirstPageHeader, FirstPageFooter, PageHeader, PageFooter
    // These seem to be specific to Guitar Pro's text formatting.

    // Skipping ScoreSystemsDefaultLayout, ScoreZoomPolicy,
    // ScoreZoom, MultiVoice.
    info.myScoreSystemsLayout =
        toIntList(splitString(node.child_value("ScoreSystemsLayout")));

    return info;
}

/// Adds the tempo changes to the appropriate master bars.
static void
parseTempoChanges(const pugi::xml_node &master_track,
                  std::vector<Gp7::MasterBar> &master_bars)
{
    for (const pugi::xml_node &node :
         master_track.child("Automations").children("Automation"))
    {
        Gp7::TempoChange change;
        change.myPosition = node.child("Position").text().as_double();
        change.myDescription = node.child_value("Text");
        change.myIsLinear = node.child("Linear").text().as_bool();
        change.myIsVisible = node.child("Visible").text().as_bool(true);

        // There should be space-separated string such as "120 2".
        std::vector<std::string> values =
            splitString(node.child_value("Value"));
        if (values.size() != 2)
            throw FileFormatException("Invalid tempo change values.");

        change.myBeatsPerMinute = std::stoi(values[0]);
        switch (std::stoi(values[1]))
        {
            using BeatType = Gp7::TempoChange::BeatType;
            case 1:
                change.myBeatType = BeatType::Eighth;
                break;
            case 2:
                change.myBeatType = BeatType::Quarter;
                break;
            case 3:
                change.myBeatType = BeatType::QuarterDotted;
                break;
            case 4:
                change.myBeatType = BeatType::Half;
                break;
            case 5:
                change.myBeatType = BeatType::HalfDotted;
                break;
            default:
                throw FileFormatException("Invalid tempo change unit.");
        }

        const int bar_idx = node.child("Bar").text().as_int();
        if (bar_idx < 0 || bar_idx >= int(master_bars.size()))
            throw FileFormatException("Invalid bar for tempo change.");

        master_bars[bar_idx].myTempoChanges.push_back(change);
    }
}

static std::vector<Gp7::Track>
parseTracks(const pugi::xml_node &tracks_node)
{
    std::vector<Gp7::Track> tracks;
    for (const pugi::xml_node &node : tracks_node.children("Track"))
    {
        Gp7::Track track;
        track.myName = node.child_value("Name");
        track.mySystemsLayout =
            toIntList(splitString(node.child_value("SystemsLayout")));

        // Many fields related to RSE are skipped here.

        // TODO - there can be two staves in a track with different tunings,
        // but which are played back using the same instrument.
        // Currently these are ignored, but we could consider importing them as
        // separate Player's.
        for (const pugi::xml_node &staff_node :
             node.child("Staves").children("Staff"))
        {
            const pugi::xml_node properties = staff_node.child("Properties");

            Gp7::Staff staff;

            // Import the capo fret.
            auto capo_property = properties.find_child_by_attribute(
                "Property", "name", "CapoFret");
            staff.myCapo = capo_property.child("Fret").text().as_int();

            // Import the tuning, which is stored as a space-separate list of
            // pitches.
            auto tuning_property = properties.find_child_by_attribute(
                "Property", "name", "Tuning");
            staff.myTuning =
                toIntList(splitString(tuning_property.child_value("Pitches")));

            track.myStaves.push_back(staff);
        }

        // Import the sounds (instruments). Many fields related to RSE are
        // skipped here.
        for (const pugi::xml_node &sound_node :
             node.child("Sounds").children("Sound"))
        {
            Gp7::Sound sound;
            sound.myLabel = sound_node.child_value("Label");
            sound.myMidiPreset =
                sound_node.child("MIDI").child("Program").text().as_int();

            track.mySounds.push_back(sound);
        }

        // TODO - import sound automations (Guitar Pro's equivalent to player /
        // instrument changes).

        tracks.push_back(track);
    }

    return tracks;
}

static std::vector<Gp7::MasterBar>
parseMasterBars(const pugi::xml_node &master_bars_node)
{
    std::vector<Gp7::MasterBar> master_bars;
    for (const pugi::xml_node &node : master_bars_node.children("MasterBar"))
    {
        Gp7::MasterBar master_bar;

        master_bar.myBarIds = toIntList(splitString(node.child_value("Bars")));

        auto section_node = node.child("Section");
        if (section_node)
        {
            Gp7::MasterBar::Section section;
            section.myLetter = section_node.child_value("Letter");
            section.myText = section_node.child_value("Text");
            master_bar.mySection = section;
        }

        if (node.child("DoubleBar"))
            master_bar.myDoubleBar = true;
        if (node.child("FreeTime"))
            master_bar.myFreeTime = true;

        if (auto repeat = node.child("Repeat"))
        {
            if (repeat.attribute("start").as_bool())
                master_bar.myRepeatStart = true;
            if (repeat.attribute("end").as_bool())
            {
                master_bar.myRepeatEnd = true;
                master_bar.myRepeatCount = repeat.attribute("count").as_int();
            }
        }

        // The time signature should be a string like 12/8.
        std::vector<int> time_sig =
            toIntList(splitString(node.child_value("Time"), '/'));
        if (time_sig.size() != 2)
            throw FileFormatException("Unexpected time signature value");

        master_bar.myTimeSig.myBeats = time_sig[0];
        master_bar.myTimeSig.myBeatValue = time_sig[1];

        // Key signature. A negative number of accidentals indicates flats.
        auto key_node = node.child("Key");
        const int accidentals =
            key_node.child("AccidentalCount").text().as_int();
        master_bar.myKeySig.myAccidentalCount = std::abs(accidentals);
        master_bar.myKeySig.myMinor =
            std::string_view(key_node.child_value("Mode")) == "Minor";
        master_bar.myKeySig.mySharps = (accidentals >= 0);

        master_bars.push_back(master_bar);
    }

    return master_bars;
}

static std::unordered_map<int, Gp7::Bar>
parseBars(const pugi::xml_node &bars_node)
{
    std::unordered_map<int, Gp7::Bar> bars;
    for (const pugi::xml_node &node : bars_node.children("Bar"))
    {
        Gp7::Bar bar;
        bar.myVoiceIds = toIntList(splitString(node.child_value("Voices")));

        using ClefType = Gp7::Bar::ClefType;
        std::string clef_name = node.child_value("Clef");
        if (clef_name == "G2")
            bar.myClefType = ClefType::G2;
        else if (clef_name == "F4")
            bar.myClefType = ClefType::F4;
        else if (clef_name == "C3")
            bar.myClefType = ClefType::C3;
        else if (clef_name == "C4")
            bar.myClefType = ClefType::C4;
        else if (clef_name == "Neutral")
            bar.myClefType = ClefType::Neutral;
        else
            throw FileFormatException("Unknown clef type");

        // TODO - import the 'Ottavia' key if the clef has 8va, etc

        const int id = node.attribute("id").as_int();
        bars.emplace(id, bar);
    }

    return bars;
}

static std::unordered_map<int, Gp7::Voice>
parseVoices(const pugi::xml_node &voices_node)
{
    std::unordered_map<int, Gp7::Voice> voices;
    for (const pugi::xml_node &node : voices_node.children("Voice"))
    {
        Gp7::Voice voice;
        voice.myBeatIds = toIntList(splitString(node.child_value("Beats")));
        const int id = node.attribute("id").as_int();
        voices.emplace(id, voice);
    }

    return voices;
}

static std::unordered_map<int, Gp7::Beat>
parseBeats(const pugi::xml_node &beats_node)
{
    std::unordered_map<int, Gp7::Beat> beats;
    for (const pugi::xml_node &node : beats_node.children("Beat"))
    {
        Gp7::Beat beat;
        beat.myRhythmId = node.child("Rhythm").attribute("ref").as_int();
        beat.myNoteIds = toIntList(splitString(node.child_value("Notes")));

        std::string_view ottavia = node.child_value("Ottavia");
        if (!ottavia.empty())
        {
            if (ottavia == "8va")
                beat.myOttavia = Gp7::Beat::Ottavia::O8va;
            else if (ottavia == "8vb")
                beat.myOttavia = Gp7::Beat::Ottavia::O8vb;
            else if (ottavia == "15ma")
                beat.myOttavia = Gp7::Beat::Ottavia::O15ma;
            else if (ottavia == "15mb")
                beat.myOttavia = Gp7::Beat::Ottavia::O15mb;
        }

        // Guitar Pro grace notes can occur before or on the beat, but we only
        // have one type.
        if (node.child("GraceNotes"))
            beat.myGraceNote = true;

        // GP tremolo picking can describe different speeds, but we just have
        // one type.
        if (node.child("Tremolo"))
            beat.myTremoloPicking = true;

        for (const pugi::xml_node property :
             node.child("Properties").children("Property"))
        {
            const std::string_view name =
                property.attribute("name").as_string();

            if (name == "Brush")
            {
                std::string_view direction = property.child_value("Direction");
                if (direction == "Up")
                    beat.myBrushUp = true;
                else if (direction == "Down")
                    beat.myBrushDown = true;
                else
                    throw FileFormatException("Unexpected brush type");
            }
        }

        std::string_view arpeggio = node.child_value("Arpeggio");
        if (!arpeggio.empty())
        {
            if (arpeggio == "Down")
                beat.myArpeggioDown = true;
            else if (arpeggio == "Up")
                beat.myArpeggioUp = true;
        }

        const int id = node.attribute("id").as_int();
        beats.emplace(id, beat);
    }

    return beats;
}

static std::unordered_map<int, Gp7::Note>
parseNotes(const pugi::xml_node &notes_node)
{
    std::unordered_map<int, Gp7::Note> notes;
    for (const pugi::xml_node &node : notes_node.children("Note"))
    {
        Gp7::Note note;

        for (const pugi::xml_node property :
             node.child("Properties").children("Property"))
        {
            const std::string_view name =
                property.attribute("name").as_string();

            if (name == "String")
                note.myString = property.child("String").text().as_int();
            else if (name == "Fret")
                note.myFret = property.child("Fret").text().as_int();
            else if (name == "PalmMuted")
                note.myPalmMuted = true;
            else if (name == "Muted")
                note.myMuted = true;
            else if (name == "Tapped")
                note.myTapped = true;
            else if (name == "HopoOrigin")
                note.myHammerOn = true;
            else if (name == "LeftHandTapped")
                note.myLeftHandTapped = true;
            else if (name == "HarmonicFret")
            {
                note.myHarmonicFret =
                    property.child("HFret").text().as_double();
            }
            else if (name == "Slide")
                note.mySlideTypes = property.child("Flags").text().as_int();
            else if (name == "HarmonicType")
            {
                using HarmonicType = Gp7::Note::HarmonicType;

                std::string_view harmonic_type = property.child_value("HType");
                if (harmonic_type == "Natural")
                    note.myHarmonic = HarmonicType::Natural;
                else if (harmonic_type == "Artificial")
                    note.myHarmonic = HarmonicType::Artificial;
                else if (harmonic_type == "Tap")
                    note.myHarmonic = HarmonicType::Tap;
                else if (harmonic_type == "Semi")
                    note.myHarmonic = HarmonicType::Semi;
                else if (harmonic_type == "Feedback")
                    note.myHarmonic = HarmonicType::Feedback;
                else
                    throw FileFormatException("Unexpected harmonic type");
            }
        }

        note.myTied = node.child("Tie").attribute("destination").as_bool();
        note.myGhost =
            std::string_view(node.child_value("AntiAccent")) == "Normal";
        note.myAccentTypes = node.child("Accent").text().as_int();

        if (auto trill = node.child("Trill"))
            note.myTrillNote = trill.text().as_int();

        std::string_view vibrato = node.child_value("Vibrato");
        if (vibrato == "Wide")
            note.myWideVibrato = true;
        else if (vibrato == "Slight")
            note.myVibrato = true;

        if (node.child("LetRing"))
            note.myLetRing = true;

        // TODO - import bends and left hand fingerings.

        const int id = node.attribute("id").as_int();
        notes.emplace(id, note);
    }

    return notes;
}

static std::unordered_map<int, Gp7::Rhythm>
parseRhythms(const pugi::xml_node &rhythms_node)
{
    static const std::unordered_map<std::string, int> theNoteValuesMap = {
        { "Whole", 1 }, { "Half", 2 },  { "Quarter", 4 }, { "Eighth", 8 },
        { "16th", 16 }, { "32nd", 32 }, { "64th", 64 }
    };

    std::unordered_map<int, Gp7::Rhythm> rhythms;
    for (const pugi::xml_node &node : rhythms_node.children("Rhythm"))
    {
        Gp7::Rhythm rhythm;

        // Import the duration.
        {
            const std::string note_value = node.child_value("NoteValue");
            auto it = theNoteValuesMap.find(note_value);
            if (it == theNoteValuesMap.end())
                throw FileFormatException("Unexpected rhythm note value");

            rhythm.myDuration = it->second;
        }

        // Dots are optional, but this will default to 0 if they're not
        // present.
        rhythm.myDots =
            node.child("AugmentationDot").attribute("count").as_int(0);

        // Import tuplets. Nested tuplets don't seem to be supported.
        pugi::xml_node tuplet = node.child("PrimaryTuplet");
        if (tuplet)
        {
            rhythm.myTupletNum = tuplet.attribute("num").as_int();
            rhythm.myTupletDenom = tuplet.attribute("den").as_int();
        }

        const int id = node.attribute("id").as_int();
        rhythms.emplace(id, rhythm);
    }

    return rhythms;
}

Gp7::Document
Gp7::parse(const pugi::xml_document &root)
{
    Gp7::Document doc;

    // Note: the following children are skipped:
    // - GPVersion
    // - GPRevision
    // - Encoding
    // These probably aren't needed unless the Guitar Pro 6 parser is merged
    // with this one.

    const pugi::xml_node gpif = root.child("GPIF");
    doc.myScoreInfo = parseScoreInfo(gpif.child("Score"));

    // Currently skipping the following children of the MasterTrack node:
    // - 'Tracks', likely not necessary since tracks are listed elsewhere
    // - 'RSE'. This is mostly to do with audio playback, but volume / pan
    // changes do occur as 'Automation' nodes here.
    const pugi::xml_node master_track = gpif.child("MasterTrack");

    doc.myTracks = parseTracks(gpif.child("Tracks"));
    doc.myMasterBars = parseMasterBars(gpif.child("MasterBars"));
    doc.myBars = parseBars(gpif.child("Bars"));
    doc.myVoices = parseVoices(gpif.child("Voices"));
    doc.myBeats = parseBeats(gpif.child("Beats"));
    doc.myNotes = parseNotes(gpif.child("Notes"));
    doc.myRhythms = parseRhythms(gpif.child("Rhythms"));

    parseTempoChanges(master_track, doc.myMasterBars);

    return doc;
}
