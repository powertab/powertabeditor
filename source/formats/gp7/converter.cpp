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

#include "converter.h"
#include "parser.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>

#include <score/score.h>
#include <score/scoreinfo.h>

/// Convert the Guitar Pro file metadata.
static void
convertScoreInfo(const Gp7::ScoreInfo &gp_info, Score &score)
{
    ScoreInfo info;
    SongData data;

    // No support for subtitle.
    // TODO - consider adding this to SongData.
    data.setTitle(gp_info.myTitle);
    data.setArtist(gp_info.myArtist);

    data.setAudioReleaseInfo(SongData::AudioReleaseInfo(
        SongData::AudioReleaseInfo::ReleaseType::Album, gp_info.myAlbum,
        boost::gregorian::day_clock::local_day().year(), false));

    // WordsAndMusic never seems to be set.
    data.setAuthorInfo(SongData::AuthorInfo(gp_info.myMusic, gp_info.myWords));
    data.setCopyright(gp_info.myCopyright);
    data.setTranscriber(gp_info.myTabber);
    data.setPerformanceNotes(gp_info.myInstructions);
    // Skipping Notices since there isn't an equivalent.

    info.setSongData(data);
    score.setScoreInfo(info);
}

/// Create players and instruments from the Guitar Pro tracks.
static void convertPlayers(const std::vector<Gp7::Track> &tracks, Score &score)
{
    for (const Gp7::Track &track : tracks)
    {
        for (const Gp7::Staff &staff : track.myStaves)
        {
            Player player;
            player.setDescription(track.myName);

            Tuning tuning;
            tuning.setNotes(std::vector<uint8_t>(staff.myTuning.rbegin(),
                                                 staff.myTuning.rend()));
            tuning.setCapo(staff.myCapo);
            player.setTuning(tuning);

            score.insertPlayer(player);
        }

        for (const Gp7::Sound &sound : track.mySounds)
        {
            Instrument instrument;
            instrument.setDescription(sound.myLabel);
            instrument.setMidiPreset(sound.myMidiPreset);

            score.insertInstrument(instrument);
        }
    }
}

void
Gp7::convert(const Gp7::Document &doc, Score &score)
{
    convertScoreInfo(doc.myScoreInfo, score);
    convertPlayers(doc.myTracks, score);
}
