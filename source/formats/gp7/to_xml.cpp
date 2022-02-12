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

static void
addValueNode(pugi::xml_node &node, const char *name, int value)
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
saveTracks(pugi::xml_node &gpif, const std::vector<Track> &tracks)
{
    // In the master track, record the space-separated list of track ids.
    auto master_track_node = gpif.append_child("MasterTrack");
    std::vector<int> ids(tracks.size());
    std::iota(ids.begin(), ids.end(), 0);
    addValueNode(master_track_node, "Tracks", listToString(ids));

    auto tracks_node = gpif.append_child("Tracks");
    int track_idx = 0;
    for (const Track &track : tracks)
    {
        auto track_node = tracks_node.append_child("Track");
        track_node.append_attribute("id").set_value(track_idx);

        addCDataNode(track_node, "Name", track.myName);

        // Set the instrument type. I'm not sure if this needs to be configured
        // differently for basses, etc, but this needs to be set to avoid being
        // interpreted as a drum track.
        auto inst_set = track_node.append_child("InstrumentSet");
        addValueNode(inst_set, "Type", "electricGuitar"s);
        addValueNode(inst_set, "LineCount", 5); // standard notation staff

        auto sounds_node = track_node.append_child("Sounds");
        for (const Sound &sound : track.mySounds)
        {
            auto sound_node = sounds_node.append_child("Sound");
            addCDataNode(sound_node, "Name", sound.myLabel);
            addCDataNode(sound_node, "Label", sound.myLabel);

            auto midi_node = sound_node.append_child("MIDI");
            addValueNode(midi_node, "LSB", 0);
            addValueNode(midi_node, "MSB", 0);
            addValueNode(midi_node, "Program", sound.myMidiPreset);
        }

        // Use MIDI playback.
        addValueNode(track_node, "AudioEngineState", "MIDI"s);

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

        // TODO - implement
        (void)bar_node;
        (void)master_bar;
    }
}

pugi::xml_document
to_xml(const Document &doc)
{
    pugi::xml_document root;

    auto gpif = root.append_child("GPIF");
    addValueNode(gpif, "GPVersion", "7.6.0"s);

    auto score = gpif.append_child("Score");
    saveScoreInfo(score, doc.myScoreInfo);

    saveTracks(gpif, doc.myTracks);
    saveMasterBars(gpif, doc.myMasterBars);

    return root;
}
} // namespace Gp7
