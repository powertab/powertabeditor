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
#include "test_serialization.h"

using namespace Score; 

TEST_CASE("Score/System/Barlines", "")
{
    System system;

    REQUIRE(system.getBarlines().size() == 2);

    Barline barline(3, Barline::SingleBar);
    system.insertBarline(barline);
    REQUIRE(system.getBarlines().size() == 3);
    REQUIRE(system.getBarlines()[1] == barline);

    system.removeBarline(barline);
    REQUIRE(system.getBarlines().size() == 2);
}

TEST_CASE("Score/System/GetPreviousBarline", "")
{
    System system;

    Barline barline(10, Barline::SingleBar);
    system.insertBarline(barline);

    REQUIRE(!system.getPreviousBarline(-1));
    REQUIRE(*system.getPreviousBarline(0) == system.getBarlines()[0]);
    REQUIRE(*system.getPreviousBarline(9) == system.getBarlines()[0]);
    REQUIRE(*system.getPreviousBarline(10) == barline);
}

TEST_CASE("Score/System/GetNextBarline", "")
{
    System system;

    Barline barline(10, Barline::SingleBar);
    system.insertBarline(barline);
    system.getBarlines()[2].setPosition(15);

    REQUIRE(*system.getNextBarline(0) == system.getBarlines()[0]);
    REQUIRE(*system.getNextBarline(5) == barline);
    REQUIRE(*system.getNextBarline(10) == barline);
    REQUIRE(*system.getNextBarline(14) == system.getBarlines()[2]);
    REQUIRE(!system.getNextBarline(16));
}

TEST_CASE("Score/System/TempoMarkers", "")
{
    System system;

    REQUIRE(system.getTempoMarkers().size() == 0);

    TempoMarker tempo1(3);
    tempo1.setBeatsPerMinute(160);
    TempoMarker tempo2(42);
    tempo2.setBeatsPerMinute(130);

    system.insertTempoMarker(tempo2);
    system.insertTempoMarker(tempo1);
    REQUIRE(system.getTempoMarkers().size() == 2);
    REQUIRE(system.getTempoMarkers()[0] == tempo1);
    REQUIRE(system.getTempoMarkers()[1] == tempo2);

    system.removeTempoMarker(tempo1);
    REQUIRE(system.getTempoMarkers().size() == 1);
    REQUIRE(system.getTempoMarkers()[0] == tempo2);
}
