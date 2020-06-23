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
  
#include <catch2/catch.hpp>

#include <actions/adddirection.h>
#include <score/score.h>

TEST_CASE("Actions/AddDirection")
{
    Score score;
    score.insertSystem(System());

    Direction direction(6);
    direction.insertSymbol(DirectionSymbol::Segno);
    ScoreLocation location(score, 0, 0, 6);
    AddDirection action(location, direction);

    action.redo();
    REQUIRE(location.getSystem().getDirections().size() == 1);
    REQUIRE(location.getSystem().getDirections()[0] == direction);

    action.undo();
    REQUIRE(location.getSystem().getDirections().size() == 0);
}
