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

#include "score/scoreinfo.h"
#include <catch2/catch.hpp>

#include <app/appinfo.h>
#include <formats/gp7/gp7importer.h>
#include <score/score.h>

TEST_CASE("Formats/Gp7Import/ScoreInfo/Basic", "")
{
    Score score;
    Gp7Importer importer;

    REQUIRE_NOTHROW(
        importer.load(AppInfo::getAbsolutePath("data/score_info.gp"), score));
    REQUIRE(score.getSystems().size() == 0);

    const ScoreInfo &info = score.getScoreInfo();
    REQUIRE(info.getScoreType() == ScoreInfo::ScoreType::Song);
    const SongData &data = info.getSongData();
    REQUIRE(data.getTitle() == "The title");
    REQUIRE(data.getArtist() == "The artist");
    REQUIRE(data.isAudioRelease());
    REQUIRE(data.getAudioReleaseInfo().getTitle() == "The album");
    REQUIRE(data.getAuthorInfo().getComposer() == "The composer");
    REQUIRE(data.getAuthorInfo().getLyricist() == "The lyricist");
    REQUIRE(data.getArranger() == "");
    REQUIRE(data.getTranscriber() == "The transcriber");
    REQUIRE(data.getCopyright() == "The copyright date");
    REQUIRE(data.getLyrics() == "");
    REQUIRE(data.getPerformanceNotes() == "The instructions");
}

// Verify that the "Words & Music" style header is imported properly.
TEST_CASE("Formats/Gp7Import/ScoreInfo/WordsAndMusic", "")
{
    Score score;
    Gp7Importer importer;

    REQUIRE_NOTHROW(
        importer.load(AppInfo::getAbsolutePath("data/words_and_music.gp"), score));

    const ScoreInfo &info = score.getScoreInfo();
    REQUIRE(info.getScoreType() == ScoreInfo::ScoreType::Song);
    const SongData &data = info.getSongData();
    REQUIRE(data.getAuthorInfo().getComposer() == "The author");
    REQUIRE(data.getAuthorInfo().getLyricist() == "The author");
}
