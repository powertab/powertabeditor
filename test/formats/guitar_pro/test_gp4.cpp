/*
  * Copyright (C) 2011 Cameron White
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

#include <catch.hpp>

#include <formats/guitar_pro/guitarproimporter.h>
#include <score/generalmidi.h>
#include <score/score.h>

const std::string FILE_TEST1 = "data/test1.gp4";

struct Gp4Fixture
{
    Gp4Fixture()
    {
        REQUIRE_NOTHROW(importer.load(FILE_TEST1, score));
    }

    GuitarProImporter importer;
    Score score;
};

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/HeaderConversion")
{
    const ScoreInfo &info = score.getScoreInfo();
    const SongData &song = info.getSongData();

    REQUIRE(song.getTitle() == "FileName");
    REQUIRE(song.getArtist() == "Artist");
    REQUIRE(song.getAudioReleaseInfo().getTitle() == "Album");
    REQUIRE(song.getAuthorInfo().getComposer() == "Author");
    REQUIRE(song.getCopyright() == "Copyright 2011");
    REQUIRE(song.getTranscriber() == "Tab Creator");
    REQUIRE(song.getPerformanceNotes()  == "Some Comments");
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/TrackImport",
                 "The players/instruments should be imported correctly.")
{
    REQUIRE(score.getPlayers().size() == 2);
    REQUIRE(score.getInstruments().size() == 2);

    const Player &player1 = score.getPlayers()[0];
    REQUIRE(player1.getDescription() == "First Track");
    REQUIRE(player1.getTuning().getCapo() == 1);
    REQUIRE(player1.getTuning().getStringCount() == 7);

    const Instrument guitar1 = score.getInstruments()[0];
    REQUIRE(guitar1.getMidiPreset() == Midi::MIDI_PRESET_OVERDRIVEN_GUITAR);
    REQUIRE(player1.getMaxVolume() == 95);
}

TEST_CASE_METHOD(Gp4Fixture,
                 "Formats/GuitarPro4Import/ReadBarline/BarlineProperties", "")
{
    const Barline &bar = score.getSystems()[0].getBarlines()[0];
    REQUIRE(bar.getBarType() == Barline::RepeatStart);
}

TEST_CASE_METHOD(Gp4Fixture,
                 "Formats/GuitarPro4Import/ReadBarline/TimeSignature", "")
{
    const Barline &bar = score.getSystems()[0].getBarlines()[0];
    const TimeSignature &time = bar.getTimeSignature();

    REQUIRE(time.getBeatsPerMeasure() == 5);
    REQUIRE(time.getBeatValue() == 4);
    REQUIRE(time.isVisible());
}

TEST_CASE_METHOD(Gp4Fixture,
                 "Formats/GuitarPro4Import/ReadBarline/RehearsalSigns", "")
{
    const System &system = score.getSystems()[0];

    // Check that rehearsal sign letters are set in sequential
    // order (A, B, C, etc), and that data is read correctly.
    const RehearsalSign &sign1 = system.getBarlines()[0].getRehearsalSign();
    REQUIRE(sign1.getDescription() == "Section 1");
    REQUIRE(sign1.getLetters() == "A");

    const RehearsalSign &sign2 = system.getBarlines()[1].getRehearsalSign();
    REQUIRE(sign2.getDescription() == "Section 2");
    REQUIRE(sign2.getLetters() == "B");
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/TempoMarkers", "")
{
    // Currently, only the initial tempo marker is imported.
    REQUIRE(score.getSystems()[0].getTempoMarkers().size() == 1);
}

TEST_CASE_METHOD(Gp4Fixture,
                 "Formats/GuitarPro4Import/NoteProperties/PositionEffects", "")
{
    // Check that the first position is tremolo picked and is tapped.
    const Position &pos = score.getSystems()[0].getStaves()[0].getVoice(0)[0];

    REQUIRE(pos.hasProperty(Position::TremoloPicking));
    REQUIRE(pos.hasProperty(Position::Tap));
    REQUIRE(pos.hasProperty(Position::PickStrokeDown));
    REQUIRE(pos.hasProperty(Position::PalmMuting));
}

TEST_CASE_METHOD(Gp4Fixture,
                 "Formats/GuitarPro4Import/NoteProperties/NoteEffects", "")
{
    const Staff &staff = score.getSystems()[0].getStaves()[0];
    const Note &note1 = staff.getVoice(0)[1].getNotes()[0];

    REQUIRE(note1.getTrilledFret() == 1);

    const Note &note2 = staff.getVoice(0)[3].getNotes()[0];
    REQUIRE(note2.hasProperty(Note::HammerOnOrPullOff));
}
