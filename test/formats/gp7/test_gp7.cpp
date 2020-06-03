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

#include <catch2/catch.hpp>

#include <app/appinfo.h>
#include <formats/gp7/gp7importer.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/scoreinfo.h>
#include <util/tostring.h>

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

    REQUIRE_NOTHROW(importer.load(
        AppInfo::getAbsolutePath("data/words_and_music.gp"), score));

    const ScoreInfo &info = score.getScoreInfo();
    REQUIRE(info.getScoreType() == ScoreInfo::ScoreType::Song);
    const SongData &data = info.getSongData();
    REQUIRE(data.getAuthorInfo().getComposer() == "The author");
    REQUIRE(data.getAuthorInfo().getLyricist() == "The author");
}

// Verify that players and instruments are imported correctly.
// This file has two tracks, but the second track has two staves.
TEST_CASE("Formats/Gp7Import/Tracks", "")
{
    Score score;
    Gp7Importer importer;

    REQUIRE_NOTHROW(
        importer.load(AppInfo::getAbsolutePath("data/tracks.gp"), score));

    REQUIRE(score.getPlayers().size() == 3);
    REQUIRE(score.getInstruments().size() == 2);

    {
        const Player &player = score.getPlayers()[0];
        REQUIRE(player.getDescription() == "Jazz Guitar");
        REQUIRE(player.getTuning().getCapo() == 2);
        REQUIRE(Util::toString(player.getTuning()) == "D A D G B E");
    }

    {
        const Instrument &instrument = score.getInstruments()[0];
        REQUIRE(instrument.getDescription() == "Jazz ES");
        REQUIRE(instrument.getMidiPreset() ==
                Midi::MIDI_PRESET_ELECTRIC_GUITAR_JAZZ);
    }

    {
        const Player &player = score.getPlayers()[1];
        REQUIRE(player.getDescription() == "Distortion Guitar");
        REQUIRE(player.getTuning().getCapo() == 0);
        REQUIRE(Util::toString(player.getTuning()) == "B E A D G B E");
    }

    {
        const Player &player = score.getPlayers()[2];
        REQUIRE(player.getDescription() == "Distortion Guitar");
        REQUIRE(player.getTuning().getCapo() == 0);
        REQUIRE(Util::toString(player.getTuning()) == "E A D G B E");
    }

    {
        const Instrument &instrument = score.getInstruments()[1];
        REQUIRE(instrument.getDescription() == "Distortion");
        REQUIRE(instrument.getMidiPreset() ==
                Midi::MIDI_PRESET_DISTORTION_GUITAR);
    }
}
