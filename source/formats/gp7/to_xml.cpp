/*
 * Copyright (C) 2022 Cameron White
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

#include "document.h"

#include <numeric>

using namespace std::string_literals;

/// Utility function to add a node with a CDATA child, i.e. <![CDATA[text]]>.
static void
addCDataNode(pugi::xml_node &node, const char *name, const std::string &value)
{
    node.append_child(name)
        .append_child(pugi::node_cdata)
        .set_value(value.c_str());
}

/// Utility function to add a node with a plain character data child.
static void
addValueNode(pugi::xml_node &node, const char *name, const std::string &value)
{
    node.append_child(name)
        .append_child(pugi::node_pcdata)
        .set_value(value.c_str());
}

template <typename T>
static void
addValueNode(pugi::xml_node &node, const char *name, T value)
{
    addValueNode(node, name, std::to_string(value));
}

template <typename T>
static std::string
listToString(const std::vector<T> &items, char sep = ' ')
{
    std::string s;
    for (size_t i = 0, n = items.size(); i < n; ++i)
    {
        if (i != 0)
            s += sep;

        s += std::to_string(items[i]);
    }

    return s;
}

namespace Gp7
{
static void
saveScoreInfo(pugi::xml_node &node, const ScoreInfo &info)
{
    addCDataNode(node, "Title", info.myTitle);
    addCDataNode(node, "SubTitle", info.mySubtitle);
    addCDataNode(node, "Artist", info.myArtist);
    addCDataNode(node, "Album", info.myAlbum);
    addCDataNode(node, "Words", info.myWords);
    addCDataNode(node, "Music", info.myMusic);
    addCDataNode(node, "Copyright", info.myCopyright);
    addCDataNode(node, "Tabber", info.myTabber);
    addCDataNode(node, "Instructions", info.myInstructions);
    addCDataNode(node, "Notices", info.myNotices);
}

static void
saveMasterTrack(pugi::xml_node &gpif, const std::vector<Track> &tracks,
                const std::vector<MasterBar> &master_bars)
{
    // In the master track, record the space-separated list of track ids.
    auto master_track = gpif.append_child("MasterTrack");
    std::vector<int> ids(tracks.size());
    std::iota(ids.begin(), ids.end(), 0);
    addValueNode(master_track, "Tracks", listToString(ids));

    // Record tempo automations
    auto automations = master_track.append_child("Automations");
    int bar_idx = 0;
    for (const MasterBar &master_bar : master_bars)
    {
        for (const TempoChange &change : master_bar.myTempoChanges)
        {
            auto node = automations.append_child("Automation");

            node.append_child("Type").text() = "Tempo";
            node.append_child("Linear").text() = false;
            node.append_child("Bar").text() = bar_idx;
            node.append_child("Position").text() = change.myPosition;
            node.append_child("Visible").text() = change.myIsVisible;
            node.append_child("Text").text() = change.myDescription.c_str();

            std::string value = std::to_string(change.myBeatsPerMinute) + " " +
                                std::to_string(int(change.myBeatType));
            node.append_child("Value").text() = value.c_str();
        }

        ++bar_idx;
    }
}

static void
saveTracks(pugi::xml_node &gpif, const std::vector<Track> &tracks)
{
    auto tracks_node = gpif.append_child("Tracks");
    int track_idx = 0;
    for (const Track &track : tracks)
    {
        auto track_node = tracks_node.append_child("Track");
        track_node.append_attribute("id").set_value(track_idx);

        addCDataNode(track_node, "Name", track.myName);

        // Seems to be important for MIDI playback to work.
        track_node.append_child("UseOneChannelPerString");

        // Set the instrument type. I'm not sure if this needs to be configured
        // differently for basses, etc, but this needs to be set to avoid being
        // interpreted as a drum track.
        {
            auto inst_set = track_node.append_child("InstrumentSet");
            addValueNode(inst_set, "Type", "electricGuitar"s);
            addValueNode(inst_set, "LineCount", 5); // standard notation staff

            auto element =
                inst_set.append_child("Elements").append_child("Element");
            addValueNode(element, "Name", "Pitched"s);
            addValueNode(element, "Type", "pitched"s);

            auto articulation = element.append_child("Articulations")
                                    .append_child("Articulation");
            addValueNode(articulation, "OutputMidiNumber", 0);
        }

        auto sounds_node = track_node.append_child("Sounds");
        for (const Sound &sound : track.mySounds)
        {
            auto sound_node = sounds_node.append_child("Sound");
            addCDataNode(sound_node, "Name", sound.myName);
            addCDataNode(sound_node, "Label", sound.myLabel);
            addValueNode(sound_node, "Path", sound.myPath);
            addValueNode(sound_node, "Role", sound.myRole);

            auto midi_node = sound_node.append_child("MIDI");
            addValueNode(midi_node, "LSB", 0);
            addValueNode(midi_node, "MSB", 0);
            addValueNode(midi_node, "Program", sound.myMidiPreset);
        }

        // Use MIDI playback.
        addValueNode(track_node, "AudioEngineState", "MIDI"s);

        // Assign MIDI channels
        {
            auto connection = track_node.append_child("MidiConnection");
            connection.append_child("Port").text() = 0;

            connection.append_child("PrimaryChannel").text() =
                track.myMidiChannel;
            connection.append_child("SecondaryChannel").text() =
                track.myMidiChannel;

            // This is a typo in GP's XML format.
            connection.append_child("ForeOneChannelPerString").text() = false;
        }

        auto staves_node = track_node.append_child("Staves");
        for (const Staff &staff : track.myStaves)
        {
            auto staff_node = staves_node.append_child("Staff");
            auto props_node = staff_node.append_child("Properties");

            auto capo = props_node.append_child("Property");
            capo.append_attribute("name").set_value("CapoFret");
            addValueNode(capo, "Fret", staff.myCapo);

            auto tuning = props_node.append_child("Property");
            tuning.append_attribute("name").set_value("Tuning");
            addValueNode(tuning, "Pitches", listToString(staff.myTuning));
        }

        // In Power Tab the notes are implicitly transposed down in the
        // standard notation staff.
        auto transpose = track_node.append_child("Transpose");
        addValueNode(transpose, "Chromatic", 0);
        addValueNode(transpose, "Octave", -1);

        auto automations = track_node.append_child("Automations");
        for (const SoundChange &change : track.mySoundChanges)
        {
            auto node = automations.append_child("Automation");

            addValueNode(node, "Type", "Sound"s);
            addValueNode(node, "Linear", "false"s);
            addValueNode(node, "Bar", change.myBar);
            addValueNode(node, "Position", change.myPosition);
            addValueNode(node, "Visible", "true"s);

            const Sound &sound = track.mySounds[change.mySoundId];
            addCDataNode(node, "Value",
                         sound.myPath + ";" + sound.myName + ";" + sound.myRole);
        }

        // TODO - export chords

        ++track_idx;
    }
}

static void
saveMasterBars(pugi::xml_node &gpif, const std::vector<MasterBar> &master_bars)
{
    auto bars_node = gpif.append_child("MasterBars");

    for (const MasterBar &master_bar : master_bars)
    {
        auto bar_node = bars_node.append_child("MasterBar");

        addValueNode(bar_node, "Bars", listToString(master_bar.myBarIds));

        if (master_bar.mySection)
        {
            auto section = bar_node.append_child("Section");
            addValueNode(section, "Letter", master_bar.mySection->myLetter);
            addValueNode(section, "Text", master_bar.mySection->myText);
        }

        // Time signature - e.g. "3/4"
        {
            std::string time_sig =
                std::to_string(master_bar.myTimeSig.myBeats) + "/" +
                std::to_string(master_bar.myTimeSig.myBeatValue);
            addValueNode(bar_node, "Time", time_sig);
        }

        // Key signature
        {
            auto key_sig = bar_node.append_child("Key");
            addValueNode(key_sig, "AccidentalCount",
                         master_bar.myKeySig.myAccidentalCount *
                             (master_bar.myKeySig.mySharps ? 1 : -1));
            addValueNode(key_sig, "Mode",
                         master_bar.myKeySig.myMinor ? "Minor"s : "Major"s);
        }

        // Bar types
        if (master_bar.myDoubleBar)
            bar_node.append_child("DoubleBar");
        if (master_bar.myFreeTime)
            bar_node.append_child("FreeTime");

        if (master_bar.myRepeatStart || master_bar.myRepeatEnd)
        {
            auto node = bar_node.append_child("Repeat");
            node.append_attribute("start").set_value(master_bar.myRepeatStart);
            node.append_attribute("end").set_value(master_bar.myRepeatEnd);
            node.append_attribute("count").set_value(master_bar.myRepeatCount);
        }

        // TODO
        // - alternate endings
        // - directions
        // - fermatas
    }
}

static void
saveBars(pugi::xml_node &gpif, const std::unordered_map<int, Bar> &bars_map)
{
    auto bars_node = gpif.append_child("Bars");

    for (auto &&[id, bar] : bars_map)
    {
        auto bar_node = bars_node.append_child("Bar");
        bar_node.append_attribute("id").set_value(id);

        // Only bass / treble clefs are needed for exporting pt2 files.
        std::string clef_str =
            (bar.myClefType == Bar::ClefType::F4) ? "F4"s : "G2"s;
        addValueNode(bar_node, "Clef", clef_str);

        addValueNode(bar_node, "Voices", listToString(bar.myVoiceIds));
    }
}

static void
saveVoices(pugi::xml_node &gpif,
           const std::unordered_map<int, Voice> &voices_map)
{
    auto voices_node = gpif.append_child("Voices");

    for (auto &&[id, voice] : voices_map)
    {
        auto voice_node = voices_node.append_child("Voice");
        voice_node.append_attribute("id").set_value(id);
        addValueNode(voice_node, "Beats", listToString(voice.myBeatIds));
    }
}

static pugi::xml_node
addPropertyNode(pugi::xml_node &props_node, const char *name)
{
    auto prop_node = props_node.append_child("Property");
    prop_node.append_attribute("name").set_value(name);
    return prop_node;
}

static void
saveBeats(pugi::xml_node &gpif, const std::unordered_map<int, Beat> &beats_map)
{
    auto beats_node = gpif.append_child("Beats");

    for (auto &&[id, beat] : beats_map)
    {
        auto beat_node = beats_node.append_child("Beat");
        beat_node.append_attribute("id").set_value(id);

        addValueNode(beat_node, "Notes", listToString(beat.myNoteIds));

        auto rhythm = beat_node.append_child("Rhythm");
        rhythm.append_attribute("ref").set_value(beat.myRhythmId);

        if (beat.myGraceNote)
            addValueNode(beat_node, "GraceNotes", "BeforeBeat"s);

        // Tremolo picking is only a 32nd note in PT currently
        if (beat.myTremoloPicking)
            addValueNode(beat_node, "Tremolo", "1/8"s);

        if (beat.myOttavia)
        {
            // TODO - unify the enum <-> string conversion with from_xml.cpp
            std::string text;
            switch (*beat.myOttavia)
            {
                case Gp7::Beat::Ottavia::O8va:
                    text = "8va";
                    break;
                case Gp7::Beat::Ottavia::O8vb:
                    text = "8vb";
                    break;
                case Gp7::Beat::Ottavia::O15ma:
                    text = "15ma";
                    break;
                case Gp7::Beat::Ottavia::O15mb:
                    text = "15mb";
                    break;
            }

            addValueNode(beat_node, "Ottavia", text);
        }

        if (beat.myArpeggioUp || beat.myArpeggioDown)
        {
            addValueNode(beat_node, "Arpeggio",
                         beat.myArpeggioUp ? "Up"s : "Down"s);
        }

        auto props_node = beat_node.append_child("Properties");
        if (beat.myBrushUp || beat.myBrushDown)
        {
            auto brush = addPropertyNode(props_node, "Brush");
            addValueNode(brush, "Direction", beat.myBrushUp ? "Up"s : "Down"s);
        }

        // TODO
        // - chord ids
        // - free text
        // - whammy
    }
}

static void
addBoolNoteProperty(pugi::xml_node &props_node, const char *name)
{
    addPropertyNode(props_node, name).append_child("Enable");
}

static void
savePitch(pugi::xml_node &props_node, const char *name,
          const Gp7::Note::Pitch &pitch)
{
    auto prop_node = addPropertyNode(props_node, name);
    auto pitch_node = prop_node.append_child("Pitch");
    addValueNode(pitch_node, "Step", std::string{ pitch.myNote });
    addValueNode(pitch_node, "Accidental", pitch.myAccidental);
    addValueNode(pitch_node, "Octave", pitch.myOctave);
}

static void
saveNotes(pugi::xml_node &gpif, const std::unordered_map<int, Note> &notes_map)
{
    auto notes_node = gpif.append_child("Notes");

    for (auto &&[id, note] : notes_map)
    {
        auto note_node = notes_node.append_child("Note");
        note_node.append_attribute("id").set_value(id);

        auto props_node = note_node.append_child("Properties");

        // String and fret.
        {
            auto prop_node = addPropertyNode(props_node, "String");
            addValueNode(prop_node, "String", note.myString);
        }
        {
            auto prop_node = addPropertyNode(props_node, "Fret");
            addValueNode(prop_node, "Fret", note.myFret);
        }

        // Record the pitch. GP ignores the note entirely if this isn't
        // present, and uses it for notation rather than computing it from the
        // tuning and string/fret.
        savePitch(props_node, "ConcertPitch", note.myConcertPitch);
        savePitch(props_node, "TransposedPitch", note.myTransposedPitch);

        if (note.myPalmMuted)
            addBoolNoteProperty(props_node, "PalmMuted");
        if (note.myMuted)
            addBoolNoteProperty(props_node, "Muted");
        if (note.myTapped)
            addBoolNoteProperty(props_node, "Tapped");
        if (note.myLeftHandTapped)
            addBoolNoteProperty(props_node, "LeftHandTapped");
        if (note.myHammerOn)
            addBoolNoteProperty(props_node, "HopoOrigin");

        if (note.mySlideTypes.any())
        {
            auto prop_node = addPropertyNode(props_node, "Slide");
            addValueNode(prop_node, "Flags", note.mySlideTypes.to_ulong());
        }

        if (note.myLetRing)
            note_node.append_child("LetRing");

        if (note.myVibrato)
            addValueNode(note_node, "Vibrato", "Slight"s);
        else if (note.myWideVibrato)
            addValueNode(note_node, "Vibrato", "Wide"s);

        if (note.myGhost)
            addValueNode(note_node, "AntiAccent", "Normal"s);

        if (note.myTrillNote)
            addValueNode(note_node, "Trill", *note.myTrillNote);

        if (note.myTieDest || note.myTieOrigin)
        {
            auto tie_node = note_node.append_child("Tie");
            tie_node.append_attribute("origin").set_value(note.myTieOrigin);
            tie_node.append_attribute("destination").set_value(note.myTieDest);
        }

        if (note.myAccentTypes.any())
            addValueNode(note_node, "Accent", note.myAccentTypes.to_ulong());

        if (note.myBend)
        {
            addBoolNoteProperty(props_node, "Bended");

            auto addBendProperty = [&](const char *name, double value)
            {
                auto prop = addPropertyNode(props_node, name);
                prop.append_child("Float").text() = value;
            };

            const Bend &bend = *note.myBend;
            addBendProperty("BendOriginValue", bend.myOriginValue);
            addBendProperty("BendOriginOffset", bend.myOriginOffset);
            addBendProperty("BendMiddleValue", bend.myMiddleValue);
            addBendProperty("BendMiddleOffset1", bend.myMiddleOffset1);
            addBendProperty("BendMiddleOffset2", bend.myMiddleOffset2);
            addBendProperty("BendDestinationValue", bend.myDestValue);
            addBendProperty("BendDestinationOffset", bend.myDestOffset);
        }

        // TODO
        // - harmonics
        // - left hand fingering
    }
}

static void
saveRhythms(pugi::xml_node &gpif,
            const std::unordered_map<int, Rhythm> &rhythms_map)
{
    static const std::unordered_map<int, std::string> theNoteNamesMap = {
        { 1, "Whole"s }, { 2, "Half"s }, { 4, "Quarter"s }, { 8, "Eighth" },
        { 16, "16th" },  { 32, "32nd" }, { 64, "64th" }
    };

    auto rhythms_node = gpif.append_child("Rhythms");

    for (auto &&[id, rhythm] : rhythms_map)
    {
        auto rhythm_node = rhythms_node.append_child("Rhythm");
        rhythm_node.append_attribute("id").set_value(id);

        addValueNode(rhythm_node, "NoteValue",
                     theNoteNamesMap.at(rhythm.myDuration));

        if (rhythm.myDots > 0)
        {
            auto dots_node = rhythm_node.append_child("AugmentationDot");
            dots_node.append_attribute("count").set_value(rhythm.myDots);
        }

        if (rhythm.myTupletDenom > 0)
        {
            auto tuplet_node = rhythm_node.append_child("PrimaryTuplet");
            tuplet_node.append_attribute("num").set_value(rhythm.myTupletNum);
            tuplet_node.append_attribute("den").set_value(rhythm.myTupletDenom);
        }
    }
}

pugi::xml_document
to_xml(const Document &doc)
{
    pugi::xml_document root;

    auto gpif = root.append_child("GPIF");
    addValueNode(gpif, "GPVersion", "7.6.0"s);

    // These values are from a recent GP version when this was implemented.
    // It needs to be sufficiently new in order for the generated XML data to
    // be interpreted correctly by GP.
    auto revision = gpif.append_child("GPRevision");
    revision.append_attribute("required").set_value("12021");
    revision.append_attribute("recommended").set_value("12023");
    revision.text().set("12025");

    auto encoding = gpif.append_child("Encoding");
    addValueNode(encoding, "EncodingDescription", "GP7"s);

    auto score = gpif.append_child("Score");
    saveScoreInfo(score, doc.myScoreInfo);

    saveMasterTrack(gpif, doc.myTracks, doc.myMasterBars);
    saveTracks(gpif, doc.myTracks);
    saveMasterBars(gpif, doc.myMasterBars);
    saveBars(gpif, doc.myBars);
    saveVoices(gpif, doc.myVoices);
    saveBeats(gpif, doc.myBeats);
    saveNotes(gpif, doc.myNotes);
    saveRhythms(gpif, doc.myRhythms);

    return root;
}
} // namespace Gp7
