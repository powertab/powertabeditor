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

#include <actions/addirregulargrouping.h>
#include <score/score.h>

TEST_CASE("Actions/AddIrregularGrouping")
{
    Score score;
    System system;
    Staff staff;
    system.insertStaff(staff);
    score.insertSystem(system);

    IrregularGrouping group(17, 5, 3, 2);
    ScoreLocation location(score, 0, 0, 6);
    AddIrregularGrouping action(location, group);

    action.redo();
    REQUIRE(location.getVoice().getIrregularGroupings().size() == 1);

    action.undo();
    REQUIRE(location.getVoice().getIrregularGroupings().size() == 0);
}
