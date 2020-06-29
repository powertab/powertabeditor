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

static void
convertBeat(const Gp::Beat &beat, Gp7::Document &gp7_doc, Gp7::Beat gp_beat)
{
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
        for (const Gp::Staff &staff : measure.myStaves)
        {
            Gp7::Bar bar;

            for (const std::vector<Gp::Beat> &voice : staff.myVoices)
            {
                Gp7::Voice gp7_voice;

                for (const Gp::Beat &beat : voice)
                {
                    Gp7::Beat gp7_beat;
                    convertBeat(beat, gp7_doc, gp7_beat);

                    const int beat_id = gp7_doc.myBeats.size();
                    gp7_doc.myBeats[beat_id] = std::move(gp7_beat);
                    gp7_voice.myBeatIds.push_back(beat_id);
                }

                const int voice_id = gp7_doc.myVoices.size();
                gp7_doc.myVoices[voice_id] = std::move(gp7_voice);
                bar.myVoiceIds.push_back(voice_id);
            }

            // GP7 expects there to be 4 voices.
            for (size_t i = bar.myVoiceIds.size(); i < 4; ++i)
                bar.myVoiceIds.push_back(-1);

            const int bar_id = gp7_doc.myBars.size();
            gp7_doc.myBars[bar_id] = std::move(bar);
            master_bar.myBarIds.push_back(bar_id);
        }

        // TODO - fill out myBarIds.

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
