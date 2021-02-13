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

#include <actions/edittempomarker.h>
#include <score/score.h>

TEST_CASE("Actions/AddTempoMarker")
{
    Score score;
    score.insertSystem(System());

    TempoMarker marker(6);
    marker.setBeatsPerMinute(160);
    ScoreLocation location(score, 0, 0, 6);
    AddTempoMarker action(location, marker);

    action.redo();
    REQUIRE(location.getSystem().getTempoMarkers().size() == 1);
    REQUIRE(location.getSystem().getTempoMarkers()[0] == marker);

    action.undo();
    REQUIRE(location.getSystem().getTempoMarkers().size() == 0);
}

TEST_CASE("Actions/RemoveTempoMarker")
{
    Score score;
    System system;
    TempoMarker marker(6);
    marker.setBeatsPerMinute(160);
    system.insertTempoMarker(marker);
    score.insertSystem(system);

    ScoreLocation location(score, 0, 0, 6);
    RemoveTempoMarker action(location);

    action.redo();
    REQUIRE(location.getSystem().getTempoMarkers().empty());

    action.undo();
    REQUIRE(location.getSystem().getTempoMarkers().size() == 1);
    REQUIRE(location.getSystem().getTempoMarkers().front() == marker);
}
