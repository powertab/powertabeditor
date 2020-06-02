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

static void convertScoreInfo(const Gp7::ScoreInfo &gp_info, Score &score)
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

void Gp7::convert(const Gp7::Document &doc, Score &score)
{
    convertScoreInfo(doc.myScoreInfo, score);
}
