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

#include <actions/addnote.h>
#include <score/score.h>

TEST_CASE("Actions/AddNote", "")
{
    Score score;
    System system;
    Staff staff;

    Position rest(7);
    rest.setRest(true);
    staff.insertPosition(0, rest);

    Position pos(8);
    pos.insertNote(Note(1, 13));
    staff.insertPosition(0, pos);

    system.insertStaff(staff);
    score.insertSystem(system);

    ScoreLocation location(score, 0, 0, 6);

    // Add a note to an empty location.
    AddNote action1(location, Note(2, 3), Position::QuarterNote);

    action1.redo();
    REQUIRE(location.getPosition());
    REQUIRE(location.getPosition()->getDurationType() == Position::QuarterNote);
    REQUIRE(location.getPosition()->getNotes().size() == 1);

    action1.undo();
    REQUIRE(!location.getPosition());

    // Add a note to a rest.
    location.setPositionIndex(7);
    AddNote action2(location, Note(2, 3), Position::QuarterNote);

    action2.redo();
    REQUIRE(location.getPosition());
    REQUIRE(!location.getPosition()->isRest());
    REQUIRE(location.getPosition()->getNotes().size() == 1);

    action2.undo();
    REQUIRE(location.getPosition());
    REQUIRE(location.getPosition()->isRest());
    REQUIRE(location.getPosition()->getNotes().size() == 0);

    // Add a note to an existing position.
    location.setPositionIndex(8);
    AddNote action3(location, Note(2, 3), Position::QuarterNote);

    action3.redo();
    REQUIRE(location.getPosition());
    REQUIRE(location.getPosition()->getNotes().size() == 2);

    action3.undo();
    REQUIRE(location.getPosition());
    REQUIRE(location.getPosition()->getNotes().size() == 1);
}
