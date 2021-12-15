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
#include <score/generalmidi.h>

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

static Gp7::Chord::Note
parseChordNote(const pugi::xml_node &node)
{
    using namespace std::string_literals;

    Gp7::Chord::Note note;
    note.myStep = node.attribute("step").as_string();

    static const std::unordered_map<std::string, int> theAccidentals = {
        { "Natural"s, 0 },
        { "Sharp"s, 1 },
        { "DoubleSharp"s, 2 },
        { "Flat"s, -1 },
        { "DoubleFlat"s, -2 }
    };

    auto it = theAccidentals.find(node.attribute("accidental").as_string());
    if (it == theAccidentals.end())
        throw FileFormatException("Unknown accidental type");

    note.myAccidental = it->second;

    return note;
}

static std::optional<Gp7::Chord::Degree>
parseChordDegree(const pugi::xml_node &chord_node, const char *name)
{
    auto node = chord_node.find_child_by_attribute("Degree", "interval", name);
    if (!node)
        return {};

    using namespace std::string_literals;
    using Alteration = Gp7::Chord::Degree::Alteration;

    Gp7::Chord::Degree degree;
    degree.myOmitted = node.attribute("omitted").as_bool();

    static const std::unordered_map<std::string, Alteration> theAlterations = {
        { "Perfect"s, Alteration::Perfect },
        { "Diminished"s, Alteration::Diminished },
        { "Augmented"s, Alteration::Augmented },
        { "Major"s, Alteration::Major },
        { "Minor"s, Alteration::Minor }
    };

    auto it = theAlterations.find(node.attribute("alteration").as_string());
    if (it == theAlterations.end())
        throw FileFormatException("Unknown alteration type");

    degree.myAlteration = it->second;
    return degree;
}

static std::unordered_map<int, Gp7::Chord>
parseChords(const pugi::xml_node &collection_node)
{
    std::unordered_map<int, Gp7::Chord> chords;

    for (auto node : collection_node.child("Items").children("Item"))
    {
        Gp7::Chord chord;

        auto chord_node = node.child("Chord");
        chord.myKeyNote = parseChordNote(chord_node.child("KeyNote"));
        chord.myBassNote = parseChordNote(chord_node.child("BassNote"));

        chord.mySecond = parseChordDegree(chord_node, "Second");
        chord.myThird = parseChordDegree(chord_node, "Third");
        chord.myFourth = parseChordDegree(chord_node, "Fourth");
        chord.myFifth = parseChordDegree(chord_node, "Fifth");
        chord.mySixth = parseChordDegree(chord_node, "Sixth");
        chord.mySeventh = parseChordDegree(chord_node, "Seventh");
        chord.myNinth = parseChordDegree(chord_node, "Ninth");
        chord.myEleventh = parseChordDegree(chord_node, "Eleventh");
        chord.myThirteenth = parseChordDegree(chord_node, "Thirteenth");

        const int id = node.attribute("id").as_int();
        chords.emplace(id, chord);
    }

    return chords;
}

static void
parseStaff(const pugi::xml_node &node, Gp7::Track &track)
{
    const pugi::xml_node properties = node.child("Properties");

    Gp7::Staff staff;

    // Import the capo fret.
    auto capo_property =
        properties.find_child_by_attribute("Property", "name", "CapoFret");
    staff.myCapo = capo_property.child("Fret").text().as_int();

    // Import the tuning, which is stored as a space-separate list
    // of pitches.
    auto tuning_property =
        properties.find_child_by_attribute("Property", "name", "Tuning");
    staff.myTuning =
        toIntList(splitString(tuning_property.child_value("Pitches")));

    auto diagram_property = properties.find_child_by_attribute(
        "Property", "name", "DiagramCollection");
    if (diagram_property)
        track.myChords = parseChords(diagram_property);

    track.myStaves.push_back(staff);
}

static std::vector<Gp7::Track>
parseTracks(const pugi::xml_node &tracks_node, Gp7::Version version)
{
    std::vector<Gp7::Track> tracks;
    for (const pugi::xml_node &node : tracks_node.children("Track"))
    {
        Gp7::Track track;
        track.myName = node.child_value("Name");
        track.mySystemsLayout =
            toIntList(splitString(node.child_value("SystemsLayout")));

        // Many fields related to RSE are skipped here.

        // .gpx files can't have multiple staves in a track.
        if (version == Gp7::Version::V6)
        {
            Gp7::Sound sound;
            sound.myMidiPreset =
                node.child("GeneralMidi").child("Program").text().as_int();
            sound.myLabel = Midi::getPresetNames().at(sound.myMidiPreset);
            track.mySounds.push_back(sound);

            parseStaff(node, track);
        }
        else
        {
            for (const pugi::xml_node &staff_node :
                 node.child("Staves").children("Staff"))
            {
                parseStaff(staff_node, track);
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

        master_bar.myAlternateEndings =
            toIntList(splitString(node.child_value("AlternateEndings")));

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

        // Fermatas.
        for (const pugi::xml_node &fermata :
             node.child("Fermatas").children("Fermata"))
        {
            std::vector<int> offset =
                toIntList(splitString(fermata.child_value("Offset"), '/'));
            if (offset.size() != 2)
                throw FileFormatException("Unexpected fermata offset.");

            master_bar.myFermatas.insert(
                boost::rational<int>(offset[0], offset[1]));
        }

        // Directions.
        if (auto dirnode = node.child("Directions"))
        {
            using DirectionTarget = Gp7::MasterBar::DirectionTarget;
            using DirectionJump = Gp7::MasterBar::DirectionJump;
            using namespace std::string_literals;

            static const std::unordered_map<std::string, DirectionTarget>
                theTargetNames = {
                    { "Fine"s, DirectionTarget::Fine },
                    { "Coda"s, DirectionTarget::Coda },
                    { "DoubleCoda"s, DirectionTarget::DoubleCoda },
                    { "Segno"s, DirectionTarget::Segno },
                    { "SegnoSegno"s, DirectionTarget::SegnoSegno },
                };

            static const std::unordered_map<std::string, DirectionJump>
                theJumpNames = {
                    { "DaCapo"s, DirectionJump::DaCapo },
                    { "DaCapoAlCoda"s, DirectionJump::DaCapoAlCoda },
                    { "DaCapoAlDoubleCoda"s,
                      DirectionJump::DaCapoAlDoubleCoda },
                    { "DaCapoAlFine"s, DirectionJump::DaCapoAlFine },
                    { "DaSegno"s, DirectionJump::DaSegno },
                    { "DaSegnoAlCoda"s, DirectionJump::DaSegnoAlCoda },
                    { "DaSegnoAlDoubleCoda"s,
                      DirectionJump::DaSegnoAlDoubleCoda },
                    { "DaSegnoAlFine"s, DirectionJump::DaSegnoAlFine },
                    { "DaSegnoSegno"s, DirectionJump::DaSegnoSegno },
                    { "DaSegnoSegnoAlCoda"s,
                      DirectionJump::DaSegnoSegnoAlCoda },
                    { "DaSegnoSegnoAlDoubleCoda"s,
                      DirectionJump::DaSegnoSegnoAlDoubleCoda },
                    { "DaSegnoSegnoAlFine"s,
                      DirectionJump::DaSegnoSegnoAlFine },
                    { "DaCoda"s, DirectionJump::DaCoda },
                    { "DaDoubleCoda"s, DirectionJump::DaDoubleCoda }
                };

            for (auto target : dirnode.children("Target"))
            {
                auto it = theTargetNames.find(target.text().as_string());
                if (it == theTargetNames.end())
                    throw FileFormatException("Invalid direction target type");

                master_bar.myDirectionTargets.push_back(it->second);
            }

            for (auto jump : dirnode.children("Jump"))
            {
                auto it = theJumpNames.find(jump.text().as_string());
                if (it == theJumpNames.end())
                    throw FileFormatException("Invalid direction jump type");

                master_bar.myDirectionJumps.push_back(it->second);
            }
        }

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
parseBeats(const pugi::xml_node &beats_node, Gp7::Version version)
{
    std::unordered_map<int, Gp7::Beat> beats;
    for (const pugi::xml_node &node : beats_node.children("Beat"))
    {
        Gp7::Beat beat;
        beat.myRhythmId = node.child("Rhythm").attribute("ref").as_int();
        beat.myNoteIds = toIntList(splitString(node.child_value("Notes")));

        if (auto chord_id = node.child("Chord"))
            beat.myChordId = chord_id.text().as_int(-1);

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

        beat.myFreeText = node.child_value("FreeText");

        // Guitar Pro grace notes can occur before or on the beat, but we only
        // have one type.
        if (node.child("GraceNotes"))
            beat.myGraceNote = true;

        // GP tremolo picking can describe different speeds, but we just have
        // one type.
        if (node.child("Tremolo"))
            beat.myTremoloPicking = true;

        Gp7::Bend whammy;
        bool has_whammy = false;
        for (const pugi::xml_node &property :
             node.child("Properties").children("Property"))
        {
            const std::string_view name =
                property.attribute("name").as_string();

            // GP6 whammy bars are stored as properties.
            if (version == Gp7::Version::V6)
            {
                auto read_float = [](const pugi::xml_node &property) {
                    return property.child("Float").text().as_double();
                };

                if (name == "WhammyBar")
                    has_whammy = true;
                else if (name == "WhammyBarOriginValue")
                    whammy.myOriginValue = read_float(property);
                else if (name == "WhammyBarOriginOffset")
                    whammy.myOriginOffset = read_float(property);
                else if (name == "WhammyBarMiddleValue")
                    whammy.myMiddleValue = read_float(property);
                else if (name == "WhammyBarMiddleOffset1")
                    whammy.myMiddleOffset1 = read_float(property);
                else if (name == "WhammyBarMiddleOffset2")
                    whammy.myMiddleOffset2 = read_float(property);
                else if (name == "WhammyBarDestinationValue")
                    whammy.myDestValue = read_float(property);
                else if (name == "WhammyBarDestinationOffset")
                    whammy.myDestOffset = read_float(property);
            }

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

        // GP7 whammy bars
        if (version != Gp7::Version::V6)
        {
            if (auto whammy_node = node.child("Whammy"))
            {
                has_whammy = true;
                whammy.myOriginValue =
                    whammy_node.attribute("originValue").as_double();
                whammy.myOriginOffset =
                    whammy_node.attribute("originOffset").as_double();
                whammy.myMiddleValue =
                    whammy_node.attribute("middleValue").as_double();
                whammy.myMiddleOffset1 =
                    whammy_node.attribute("middleOffset1").as_double();
                whammy.myMiddleOffset2 =
                    whammy_node.attribute("middleOffset2").as_double();
                whammy.myDestValue =
                    whammy_node.attribute("destinationValue").as_double();
                whammy.myDestOffset =
                    whammy_node.attribute("destinationOffset").as_double();
            }
        }

        if (has_whammy)
            beat.myWhammy = whammy;

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

        Gp7::Bend bend;
        bool has_bend = false;
        for (const pugi::xml_node &property :
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
            else if (name == "Bended")
                has_bend = true;
            else if (name == "BendOriginValue")
                bend.myOriginValue = property.child("Float").text().as_double();
            else if (name == "BendOriginOffset")
            {
                bend.myOriginOffset =
                    property.child("Float").text().as_double();
            }
            else if (name == "BendMiddleValue")
                bend.myMiddleValue = property.child("Float").text().as_double();
            else if (name == "BendMiddleOffset1")
            {
                bend.myMiddleOffset1 =
                    property.child("Float").text().as_double();
            }
            else if (name == "BendMiddleOffset2")
            {
                bend.myMiddleOffset2 =
                    property.child("Float").text().as_double();
            }
            else if (name == "BendDestinationValue")
                bend.myDestValue = property.child("Float").text().as_double();
            else if (name == "BendDestinationOffset")
                bend.myDestOffset = property.child("Float").text().as_double();
        }

        if (has_bend)
            note.myBend = bend;

        if (auto tie = node.child("Tie"))
        {
            note.myTieOrigin = tie.attribute("origin").as_bool();
            note.myTieDest = tie.attribute("destination").as_bool();
        }

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

        if (auto fingering = node.child("LeftFingering"))
        {
            using FingerType = Gp7::Note::FingerType;

            std::string_view text = fingering.text().as_string();
            if (text == "Open")
                note.myLeftFinger = FingerType::Open;
            else if (text == "C")
                note.myLeftFinger = FingerType::C;
            else if (text == "A")
                note.myLeftFinger = FingerType::A;
            else if (text == "M")
                note.myLeftFinger = FingerType::M;
            else if (text == "I")
                note.myLeftFinger = FingerType::I;
            else if (text == "P")
                note.myLeftFinger = FingerType::P;
            else
                throw FileFormatException("Unexpected finger type.");
        }

        // TODO - import bends.

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

void
Gp7::Document::addBar(MasterBar &master_bar, Bar bar)
{
    const int bar_id = static_cast<int>(myBars.size());
    myBars[bar_id] = std::move(bar);
    master_bar.myBarIds.push_back(bar_id);
}

void
Gp7::Document::addVoice(Bar &bar, Voice voice)
{
    const int voice_id = static_cast<int>(myVoices.size());
    myVoices[voice_id] = std::move(voice);
    bar.myVoiceIds.push_back(voice_id);
}

void
Gp7::Document::addBeat(Voice &voice, Beat beat)
{
    const int beat_id = static_cast<int>(myBeats.size());
    myBeats[beat_id] = std::move(beat);
    voice.myBeatIds.push_back(beat_id);
}

void
Gp7::Document::addNote(Beat &beat, Note note)
{
    const int note_id = static_cast<int>(myNotes.size());
    myNotes[note_id] = std::move(note);
    beat.myNoteIds.push_back(note_id);
}

void
Gp7::Document::addRhythm(Beat &beat, Rhythm rhythm)
{
    // TODO - consolidate identical rhythms?
    const int rhythm_id = static_cast<int>(myRhythms.size());
    myRhythms[rhythm_id] = std::move(rhythm);
    beat.myRhythmId = rhythm_id;
}

Gp7::Document
Gp7::parse(const pugi::xml_document &root, Version version)
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

    doc.myTracks = parseTracks(gpif.child("Tracks"), version);
    doc.myMasterBars = parseMasterBars(gpif.child("MasterBars"));
    doc.myBars = parseBars(gpif.child("Bars"));
    doc.myVoices = parseVoices(gpif.child("Voices"));
    doc.myBeats = parseBeats(gpif.child("Beats"), version);
    doc.myNotes = parseNotes(gpif.child("Notes"));
    doc.myRhythms = parseRhythms(gpif.child("Rhythms"));

    parseTempoChanges(master_track, doc.myMasterBars);

    return doc;
}
