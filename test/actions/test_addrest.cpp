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

#include <doctest/doctest.h>

#include <actions/addrest.h>
#include <score/score.h>

TEST_CASE("Actions/AddRest")
{
    Score score;
    System system;
    Staff staff;
    Position pos(7, Position::EighthNote);
    pos.insertNote(Note(1, 2));
    staff.getVoices()[0].insertPosition(pos);
    system.insertStaff(staff);
    score.insertSystem(system);

    ScoreLocation location(score, 0, 0, 6);

    // Add a rest to an empty location.
    AddRest action1(location, Position::HalfNote);

    action1.redo();
    REQUIRE(location.getPosition());
    REQUIRE(location.getPosition()->getDurationType() == Position::HalfNote);
    REQUIRE(location.getPosition()->isRest());

    action1.undo();
    REQUIRE(!location.getPosition());

    // Add rest to an existing position.
    location.setPositionIndex(7);
    AddRest action2(location, Position::SixteenthNote);

    action2.redo();
    REQUIRE(location.getPosition());
    REQUIRE(location.getPosition()->isRest());
    REQUIRE(location.getPosition()->getDurationType() == Position::SixteenthNote);
    REQUIRE(location.getPosition()->getNotes().size() == 0);

    action2.undo();
    REQUIRE(location.getPosition());
    REQUIRE(!location.getPosition()->isRest());
    REQUIRE(location.getPosition()->getDurationType() == Position::EighthNote);
    REQUIRE(location.getPosition()->getNotes().size() == 1);
}
