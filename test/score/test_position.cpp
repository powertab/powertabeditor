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

#include <score/position.h>
#include "test_serialization.h"

using namespace Score; 

TEST_CASE("Score/Position/SimpleProperties", "")
{
    Position position;

    REQUIRE(!position.hasProperty(Position::PalmMuting));
    position.setProperty(Position::PalmMuting, true);
    REQUIRE(position.hasProperty(Position::PalmMuting));
}

TEST_CASE("Score/Position/Serialization", "")
{
    Position position;
    position.setPosition(42);
    position.setDurationType(Position::HalfNote);
    position.setProperty(Position::PalmMuting, true);
    position.setProperty(Position::WideVibrato, true);

    Serialization::test(position);
}
