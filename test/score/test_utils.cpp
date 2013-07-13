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

#include <score/system.h>
#include <score/utils.h>

TEST_CASE("Score/Utils/FindByPosition", "")
{
    System system;

    Barline barline(42, Barline::SingleBar);
    system.insertBarline(barline);

    REQUIRE(!ScoreUtils::findByPosition(system.getBarlines(), 5));
    REQUIRE(ScoreUtils::findByPosition(system.getBarlines(), 0)); // Start bar.
    REQUIRE(*ScoreUtils::findByPosition(system.getBarlines(), 42) == barline);
}
