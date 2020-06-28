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

static Gp7::ScoreInfo
convertScoreInfo(const Gp::Header &header)
{
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

    // myScoreSystemsLayout is left empty - there isn't really an equivalent in
    // the gp5 file format.

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
        // Not setting mySystemsLayout, since there is no GP5 equivalent.

        Gp7::Staff staff;
        staff.myCapo = track.myCapo;
        for (uint8_t note : track.myTuning)
            staff.myTuning.push_back(note);

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

Gp7::Document
Gp::convertToGp7(const Gp::Document &doc)
{
    Gp7::Document gp7_doc;
    gp7_doc.myScoreInfo = convertScoreInfo(doc.myHeader);
    gp7_doc.myTracks = convertTracks(doc);
    return gp7_doc;
}
