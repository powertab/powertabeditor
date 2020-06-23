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

#include <score/system.h>

TEST_CASE("Score/System/Staves")
{
    System system;

    REQUIRE(system.getStaves().size() == 0);

    Staff staff;
    system.insertStaff(staff);
    REQUIRE(system.getStaves().size() == 1);

    system.removeStaff(0);
    REQUIRE(system.getStaves().size() == 0);
}

TEST_CASE("Score/System/Barlines")
{
    System system;

    REQUIRE(system.getBarlines().size() == 2);

    Barline barline(3, Barline::SingleBar);
    system.insertBarline(barline);
    REQUIRE(system.getBarlines().size() == 3);
    REQUIRE(system.getBarlines()[1] == barline);

    system.removeBarline(barline);
    REQUIRE(system.getBarlines().size() == 2);
}

TEST_CASE("Score/System/GetPreviousBarline")
{
    System system;

    Barline barline(10, Barline::SingleBar);
    system.insertBarline(barline);

    REQUIRE(!system.getPreviousBarline(0));
    REQUIRE(*system.getPreviousBarline(1) == system.getBarlines()[0]);
    REQUIRE(*system.getPreviousBarline(10) == system.getBarlines()[0]);
    REQUIRE(*system.getPreviousBarline(11) == barline);
}

TEST_CASE("Score/System/GetNextBarline")
{
    System system;

    Barline barline(10, Barline::SingleBar);
    system.insertBarline(barline);
    system.getBarlines()[2].setPosition(15);

    REQUIRE(*system.getNextBarline(0) == barline);
    REQUIRE(*system.getNextBarline(10) == system.getBarlines()[2]);
    REQUIRE(!system.getNextBarline(15));
}

TEST_CASE("Score/System/TempoMarkers")
{
    System system;

    REQUIRE(system.getTempoMarkers().size() == 0);

    TempoMarker tempo1(3);
    tempo1.setBeatsPerMinute(160);
    TempoMarker tempo2(42);
    tempo2.setBeatsPerMinute(130);

    system.insertTempoMarker(tempo2);
    system.insertTempoMarker(tempo1);
    REQUIRE(system.getTempoMarkers().size() == 2);
    REQUIRE(system.getTempoMarkers()[0] == tempo1);
    REQUIRE(system.getTempoMarkers()[1] == tempo2);

    system.removeTempoMarker(tempo1);
    REQUIRE(system.getTempoMarkers().size() == 1);
    REQUIRE(system.getTempoMarkers()[0] == tempo2);
}

TEST_CASE("Score/System/AlternateEndings")
{
    System system;

    REQUIRE(system.getAlternateEndings().size() == 0);

    AlternateEnding ending1;
    ending1.setPosition(3);
    AlternateEnding ending2;
    ending2.setPosition(5);

    system.insertAlternateEnding(ending2);
    system.insertAlternateEnding(ending1);
    REQUIRE(system.getAlternateEndings().size() == 2);
    REQUIRE(system.getAlternateEndings()[0] == ending1);
    REQUIRE(system.getAlternateEndings()[1] == ending2);

    system.removeAlternateEnding(ending1);
    REQUIRE(system.getAlternateEndings().size() == 1);
    REQUIRE(system.getAlternateEndings()[0] == ending2);
}

TEST_CASE("Score/System/Directions")
{
    System system;

    REQUIRE(system.getDirections().size() == 0);

    Direction direction1;
    direction1.setPosition(3);
    Direction direction2;
    direction2.setPosition(5);

    system.insertDirection(direction2);
    system.insertDirection(direction1);
    REQUIRE(system.getDirections().size() == 2);
    REQUIRE(system.getDirections()[0] == direction1);
    REQUIRE(system.getDirections()[1] == direction2);

    system.removeDirection(direction1);
    REQUIRE(system.getDirections().size() == 1);
    REQUIRE(system.getDirections()[0] == direction2);
}

TEST_CASE("Score/System/PlayerChanges")
{
    System system;

    REQUIRE(system.getPlayerChanges().size() == 0);

    PlayerChange change1;
    change1.setPosition(3);
    PlayerChange change2;
    change2.setPosition(5);

    system.insertPlayerChange(change2);
    system.insertPlayerChange(change1);
    REQUIRE(system.getPlayerChanges().size() == 2);
    REQUIRE(system.getPlayerChanges()[0] == change1);
    REQUIRE(system.getPlayerChanges()[1] == change2);

    system.removePlayerChange(change1);
    REQUIRE(system.getPlayerChanges().size() == 1);
    REQUIRE(system.getPlayerChanges()[0] == change2);
}

TEST_CASE("Score/System/Chords")
{
    System system;

    REQUIRE(system.getChords().size() == 0);

    ChordText chord1;
    chord1.setPosition(3);
    ChordText chord2;
    chord2.setPosition(5);

    system.insertChord(chord2);
    system.insertChord(chord1);
    REQUIRE(system.getChords().size() == 2);
    REQUIRE(system.getChords()[0] == chord1);
    REQUIRE(system.getChords()[1] == chord2);

    system.removeChord(chord1);
    REQUIRE(system.getChords().size() == 1);
    REQUIRE(system.getChords()[0] == chord2);
}

TEST_CASE("Score/System/TextItems")
{
    System system;

    REQUIRE(system.getTextItems().size() == 0);

    TextItem text1(3, "foo");
    TextItem text2(1, "bar");

    system.insertTextItem(text1);
    system.insertTextItem(text2);
    REQUIRE(system.getTextItems().size() == 2);
    REQUIRE(system.getTextItems()[0] == text2);
    REQUIRE(system.getTextItems()[1] == text1);

    system.removeTextItem(text2);
    REQUIRE(system.getTextItems().size() == 1);
    REQUIRE(system.getTextItems()[0] == text1);
}
