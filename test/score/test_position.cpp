/*
  * Copyright (C) 2013 Cameron White
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
  
#include <doctest/doctest.h>

#include <score/position.h>
#include "test_serialization.h"

TEST_CASE("Score/Position/SimpleProperties")
{
    Position position;

    REQUIRE(!position.hasProperty(Position::PalmMuting));
    position.setProperty(Position::PalmMuting, true);
    REQUIRE(position.hasProperty(Position::PalmMuting));
}

TEST_CASE("Score/Position/MultiBarRest")
{
    Position position;

    REQUIRE(!position.hasMultiBarRest());
    REQUIRE_THROWS(position.getMultiBarRestCount());

    position.setMultiBarRest(7);
    REQUIRE(position.hasMultiBarRest());
    REQUIRE(position.getMultiBarRestCount() == 7);

    position.clearMultiBarRest();
    REQUIRE(!position.hasMultiBarRest());
    REQUIRE_THROWS(position.getMultiBarRestCount());
}

TEST_CASE("Score/Position/Notes")
{
    Position position;
    REQUIRE(position.getNotes().size() == 0);

    Note note1(2, 12), note2(4, 13);

    position.insertNote(note2);
    position.insertNote(note1);

    REQUIRE(position.getNotes().size() == 2);
    REQUIRE(position.getNotes()[0] == note1);
    REQUIRE(position.getNotes()[1] == note2);

    position.removeNote(note1);
    REQUIRE(position.getNotes().size() == 1);
    REQUIRE(position.getNotes()[0] == note2);
}

TEST_CASE("Score/Position/FindByString")
{
    Position position;
    Note note1(2, 12), note2(4, 13);
    position.insertNote(note2);
    position.insertNote(note1);

    REQUIRE(!Utils::findByString(position, 3));
    REQUIRE(*Utils::findByString(position, 2) == note1);
    REQUIRE(*Utils::findByString(position, 4) == note2);
}

TEST_CASE("Score/Position/HasNoteWithTappedHarmonic")
{
    Position position;
    REQUIRE(!Utils::hasNoteWithTappedHarmonic(position));

    Note note;
    note.setTappedHarmonicFret(12);
    position.insertNote(note);
    REQUIRE(Utils::hasNoteWithTappedHarmonic(position));
}

TEST_CASE("Score/Position/HasNoteWithArtificialHarmonic")
{
    Position position;
    REQUIRE(!Utils::hasNoteWithArtificialHarmonic(position));

    Note note;
    note.setArtificialHarmonic(ArtificialHarmonic(ChordName::D, ChordName::Flat,
            ArtificialHarmonic::Octave::Octave15ma));
    position.insertNote(note);
    REQUIRE(Utils::hasNoteWithArtificialHarmonic(position));
}

TEST_CASE("Score/Position/HasNoteWithTrill")
{
    Position position;
    REQUIRE(!Utils::hasNoteWithTrill(position));

    Note note;
    note.setTrilledFret(12);
    position.insertNote(note);
    REQUIRE(Utils::hasNoteWithTrill(position));
}

TEST_CASE("Score/Position/HasNoteWithBend")
{
    Position position;
    REQUIRE(!Utils::hasNoteWithBend(position));

    Note note;
    note.setBend(
        Bend(Bend::BendAndHold, 2, 0, 0, Bend::LowPoint, Bend::MidPoint));
    position.insertNote(note);
    REQUIRE(Utils::hasNoteWithBend(position));
}

TEST_CASE("Score/Position/HasNoteWithProperty")
{
    Position position;

    REQUIRE(!Utils::hasNoteWithProperty(position, Note::HammerOnOrPullOff));

    Note note;
    note.setProperty(Note::GhostNote);
    position.insertNote(note);
    REQUIRE(!Utils::hasNoteWithProperty(position, Note::HammerOnOrPullOff));
    position.removeNote(note);

    note.setProperty(Note::HammerOnOrPullOff);
    position.insertNote(note);
    REQUIRE(Utils::hasNoteWithProperty(position, Note::HammerOnOrPullOff));
}

TEST_CASE("Score/Position/VolumeSwell")
{
    Position pos;

    REQUIRE(!pos.hasVolumeSwell());

    const VolumeSwell swell(VolumeLevel::Off, VolumeLevel::fff, 5);
    pos.setVolumeSwell(swell);
    REQUIRE(pos.hasVolumeSwell());
    REQUIRE(pos.getVolumeSwell().getDuration() == 5);
    REQUIRE(pos.getVolumeSwell().getEndVolume() == VolumeLevel::fff);

    pos.clearVolumeSwell();
    REQUIRE(!pos.hasVolumeSwell());
}

TEST_CASE("Score/Position/TremoloBar")
{
    Position pos;

    REQUIRE(!pos.hasTremoloBar());

    const TremoloBar bar(TremoloBar::Type::Release, 7,  5);
    pos.setTremoloBar(bar);
    REQUIRE(pos.hasTremoloBar());
    REQUIRE(pos.getTremoloBar().getDuration() == 5);
    REQUIRE(pos.getTremoloBar().getPitch() == 7);
    REQUIRE(pos.getTremoloBar().getType() == TremoloBar::Type::Release);

    pos.clearTremoloBar();
    REQUIRE(!pos.hasTremoloBar());
}

TEST_CASE("Score/Position/TremoloBar/GetPitchText")
{
    REQUIRE(TremoloBar::getPitchText(0) == "Standard");
    REQUIRE(TremoloBar::getPitchText(4) == "1");
    REQUIRE(TremoloBar::getPitchText(2) == "1/2");
    REQUIRE(TremoloBar::getPitchText(3) == "3/4");
    REQUIRE(TremoloBar::getPitchText(7) == "1 3/4");
    REQUIRE(TremoloBar::getPitchText(8) == "2");
}

TEST_CASE("Score/Position/Serialization")
{
    Position position;
    position.setPosition(42);
    position.setDurationType(Position::HalfNote);
    position.setProperty(Position::PalmMuting, true);
    position.setProperty(Position::WideVibrato, true);
    position.setMultiBarRest(7);
    position.setVolumeSwell(VolumeSwell(VolumeLevel::Off, VolumeLevel::fff));

    Serialization::test("position", position);
}
