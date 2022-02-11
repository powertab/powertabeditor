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

#include "from_pt2.h"

#include "document.h"

#include <score/score.h>
#include <score/scoreinfo.h>

/// Convert the Guitar Pro file metadata.
static Gp7::ScoreInfo
convertScoreInfo(const ScoreInfo &info)
{
    Gp7::ScoreInfo gp_info;

    if (info.getScoreType() == ScoreInfo::ScoreType::Song)
    {
        const SongData &song_data = info.getSongData();
        gp_info.myTitle = song_data.getTitle();
        gp_info.mySubtitle = song_data.getSubtitle();
        gp_info.myArtist = song_data.getArtist();

        if (song_data.isAudioRelease())
            gp_info.myAlbum = song_data.getAudioReleaseInfo().getTitle();
        else if (song_data.isVideoRelease())
            gp_info.myAlbum = song_data.getVideoReleaseInfo().getTitle();
        else if (song_data.isBootleg())
            gp_info.myAlbum = song_data.getBootlegInfo().getTitle();

        if (!song_data.isTraditionalAuthor())
        {
            gp_info.myMusic = song_data.getAuthorInfo().getComposer();
            gp_info.myWords = song_data.getAuthorInfo().getLyricist();
        }

        gp_info.myCopyright = song_data.getCopyright();
        gp_info.myTabber = song_data.getTranscriber();
        gp_info.myInstructions = song_data.getPerformanceNotes();
    }
    else
    {
        const LessonData &lesson_data = info.getLessonData();
        gp_info.myTitle = lesson_data.getTitle();
        gp_info.mySubtitle = lesson_data.getSubtitle();

        gp_info.myMusic = lesson_data.getAuthor();
        gp_info.myCopyright = lesson_data.getCopyright();
        gp_info.myInstructions = lesson_data.getNotes();
    }

    return gp_info;
}

static std::vector<Gp7::Track>
convertTracks(const Score &score)
{
    std::vector<Gp7::Track> tracks;

    for (const Player &player : score.getPlayers())
    {
        Gp7::Track track;
        track.myName = player.getDescription();

        // TODO - export only instruments that this player uses in the score?
        for (const Instrument &inst : score.getInstruments())
        {
            Gp7::Sound sound;
            sound.myLabel = inst.getDescription();
            sound.myMidiPreset = inst.getMidiPreset();

            track.mySounds.push_back(sound);
        }

        const Tuning &tuning = player.getTuning();
        const std::vector<uint8_t> &notes = tuning.getNotes();

        Gp7::Staff staff;
        staff.myCapo = tuning.getCapo();
        staff.myTuning.assign(notes.rbegin(), notes.rend());
        track.myStaves.push_back(staff);

        // TODO - export chords

        tracks.push_back(std::move(track));
    }

    return tracks;
}

Gp7::Document
Gp7::convert(const Score &score)
{
    Gp7::Document gp_doc;
    gp_doc.myScoreInfo = convertScoreInfo(score.getScoreInfo());

    gp_doc.myTracks = convertTracks(score);

    // TODO - convert master bars. For now, one is needed for tracks to appear
    // in GP.
    MasterBar master_bar;
    gp_doc.myMasterBars.push_back(master_bar);

    return gp_doc;
}
