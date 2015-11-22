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
  
#include <catch.hpp>

#include <actions/addsystem.h>
#include <score/score.h>

TEST_CASE("Actions/AddSystem", "")
{
    Score score;
    System system;
    system.insertStaff(Staff(7));
    system.insertStaff(Staff(5));
    score.insertSystem(system);
    score.insertSystem(system);

    AddSystem action(score, 1);

    action.redo();
    REQUIRE(score.getSystems().size() == 3);
    REQUIRE(score.getSystems()[0].getStaves().size() == 2);
    REQUIRE(score.getSystems()[1].getStaves().size() == 2);
    REQUIRE(score.getSystems()[1].getStaves()[0].getStringCount() == 7);
    REQUIRE(score.getSystems()[1].getStaves()[1].getStringCount() == 5);
    REQUIRE(score.getSystems()[2].getStaves().size() == 2);

    action.undo();
    REQUIRE(score.getSystems().size() == 2);
}
