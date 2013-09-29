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

#include <score/timesignature.h>
#include "test_serialization.h"

TEST_CASE("Score/TimeSignature/CutTime", "")
{
    TimeSignature time;
    time.setMeterType(TimeSignature::CutTime);

    REQUIRE(time.getMeterType() == TimeSignature::CutTime);
    REQUIRE(time.getBeatsPerMeasure() == 2);
    REQUIRE(time.getBeatValue() == 2);
}

TEST_CASE("Score/TimeSignature/Pulses", "")
{
    TimeSignature time;

    time.setBeatsPerMeasure(6);

    REQUIRE(time.isValidNumPulses(0));
    REQUIRE(time.isValidNumPulses(1));
    REQUIRE(time.isValidNumPulses(2));
    REQUIRE(time.isValidNumPulses(3));
    REQUIRE(time.isValidNumPulses(6));

    REQUIRE(!time.isValidNumPulses(4));
    REQUIRE(!time.isValidNumPulses(7));
    REQUIRE(!time.isValidNumPulses(12));
}

TEST_CASE("Score/TimeSignature/BeatAmount", "")
{
    TimeSignature time;

    time.setBeatValue(2);
    REQUIRE(time.getBeatValue() == 2);
    time.setBeatValue(32);
    REQUIRE(time.getBeatValue() == 32);
    time.setBeatValue(1);
    REQUIRE(time.getBeatValue() == 1);
}

TEST_CASE("Score/TimeSignature/Serialization", "")
{
    TimeSignature time;
    time.setMeterType(TimeSignature::CutTime);
    time.setBeatsPerMeasure(5);
    time.setBeatValue(8);
    TimeSignature::BeamingPattern pattern = {{3,2,0,0}};
    time.setBeamingPattern(pattern);
    time.setNumPulses(5);
    time.setVisible(false);

    Serialization::test("time_signature", time);
}
