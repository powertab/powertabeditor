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

#include <score/tempomarker.h>
#include "test_serialization.h"

TEST_CASE("Score/TempoMarker/BeatsPerMinute", "")
{
    TempoMarker tempo;

    tempo.setBeatsPerMinute(140);
    REQUIRE(tempo.getBeatsPerMinute() == 140);

    REQUIRE_THROWS(tempo.setBeatsPerMinute(0));
    REQUIRE_THROWS(tempo.setBeatsPerMinute(-2));
    REQUIRE_THROWS(tempo.setBeatsPerMinute(400));
}

TEST_CASE("Score/TempoMarker/Serialization", "")
{
    TempoMarker tempo(42);
    tempo.setMarkerType(TempoMarker::AlterationOfPace);
    tempo.setBeatType(TempoMarker::Sixteenth);
    tempo.setListessoBeatType(TempoMarker::Eighth);
    tempo.setTripletFeel(TempoMarker::TripletFeelEighth);
    tempo.setAlterationOfPace(TempoMarker::Ritardando);
    tempo.setBeatsPerMinute(140);
    tempo.setDescription("My Tempo");

    Serialization::test("tempo_marker", tempo);
}
