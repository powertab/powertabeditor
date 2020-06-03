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

static std::vector<std::string>
splitString(std::string input)
{
    std::vector<std::string> output;
    boost::algorithm::split(output, input, [](char c) { return c == ' '; });
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

    // Skipping ScoreSystemsDefaultLayout, ScoreSystemsLayout, ScoreZoomPolicy,
    // ScoreZoom, MultiVoice.
    // TODO - will any of these help to preserve the original file's layout?

    return info;
}

static std::vector<Gp7::TempoChange>
parseTempoChanges(const pugi::xml_node &master_track)
{
    std::vector<Gp7::TempoChange> changes;
    for (const pugi::xml_node &node :
         master_track.child("Automations").children("Automation"))
    {
        Gp7::TempoChange change;
        change.myBar = node.child("Bar").text().as_int();
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
    }
    return changes;
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
    doc.myTempoChanges = parseTempoChanges(master_track);

    return doc;
}
