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

#include <powertabdocument/timesignature.h>

TEST_CASE("PowerTabDocument/TimeSignature/ValidPulses", "")
{
    TimeSignature time;

    time.SetBeatsPerMeasure(6);

    REQUIRE(time.IsValidPulses(0));
    REQUIRE(time.IsValidPulses(1));
    REQUIRE(time.IsValidPulses(2));
    REQUIRE(time.IsValidPulses(3));
    REQUIRE(time.IsValidPulses(6));

    REQUIRE(!time.IsValidPulses(4));
    REQUIRE(!time.IsValidPulses(7));
    REQUIRE(!time.IsValidPulses(12));
}

TEST_CASE("PowerTabDocument/TimeSignature/BeatAmount", "")
{
    TimeSignature time;

    time.SetBeatAmount(2);
    REQUIRE(time.GetBeatAmount() == 2);
    time.SetBeatAmount(32);
    REQUIRE(time.GetBeatAmount() == 32);
    time.SetBeatAmount(1);
    REQUIRE(time.GetBeatAmount() == 1);
}
