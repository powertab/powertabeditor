
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

#include <score/barline.h>
#include "test_serialization.h"

TEST_CASE("Score/Barline/RehearsalSign")
{
    Barline barline;

    REQUIRE(!barline.hasRehearsalSign());

    barline.setRehearsalSign(RehearsalSign("D", "Solo"));
    REQUIRE(barline.hasRehearsalSign());
    REQUIRE(barline.getRehearsalSign().getDescription() == "Solo");
}

TEST_CASE("Score/Barline/Serialization")
{
    Barline barline;
    barline.setBarType(Barline::RepeatEnd);
    barline.setRepeatCount(42);
    barline.setPosition(23);

    Serialization::test("barline", barline);

    barline.setRehearsalSign(RehearsalSign("D", "Solo"));
	Serialization::test("barline", barline);
}
