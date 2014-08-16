/*
  * Copyright (C) 2012 Cameron White
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

#include <actions/removeposition.h>
#include "actionfixture.h"

TEST_CASE_METHOD(ActionFixture, "Actions/RemovePosition", "")
{
    RemovePosition action(myLocation);

    action.redo();
    REQUIRE(myLocation.getPosition() == NULL);

    action.undo();
    REQUIRE(myLocation.getPosition() != NULL);
    REQUIRE(myLocation.getPosition()->getNotes().size() == 2);
    REQUIRE(myLocation.getNote() != NULL);
}

TEST_CASE("Actions/RemovePositionAndGroups", "")
{
    Score score;
    System system;
    Staff staff(6);

    Position pos1(3);
    Position pos2(5);
    Position pos3(7);
    Position pos4(9);
    staff.getVoices()[0].insertPosition(pos1);
    staff.getVoices()[0].insertPosition(pos2);
    staff.getVoices()[0].insertPosition(pos3);
    staff.getVoices()[0].insertPosition(pos4);
    staff.getVoices()[0].insertIrregularGrouping(IrregularGrouping(3, 3, 3, 2));
    staff.getVoices()[0].insertIrregularGrouping(IrregularGrouping(5, 3, 3, 2));
    system.insertStaff(staff);
    score.insertSystem(system);

    ScoreLocation location(score);

    location.setPositionIndex(5);
    RemovePosition action1(location);
    location.setPositionIndex(7);
    RemovePosition action2(location);

    action1.redo();
    REQUIRE(location.getVoice().getPositions().size() == 3);
    REQUIRE(location.getVoice().getIrregularGroupings().size() == 0);

    action2.redo();
    REQUIRE(location.getVoice().getPositions().size() == 2);
    REQUIRE(location.getVoice().getIrregularGroupings().size() == 0);

    action2.undo();
    REQUIRE(location.getVoice().getPositions().size() == 3);
    REQUIRE(location.getVoice().getIrregularGroupings().size() == 0);

    action1.undo();
    REQUIRE(location.getVoice().getPositions().size() == 4);
    REQUIRE(location.getVoice().getIrregularGroupings().size() == 2);
}
