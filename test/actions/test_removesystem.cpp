/*
  * Copyright (C) 2012 Cameron White
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

#include <boost/make_shared.hpp>

#include "../powertabdocument/score_fixture.h"
#include <actions/removesystem.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/dynamic.h>
#include <powertabdocument/tempomarker.h>

TEST_CASE_METHOD(ScoreFixture, "Actions/RemoveSystem",
"Tempo markers and other symbols should also be removed when deleting a system")
{
    score.InsertTempoMarker(boost::make_shared<TempoMarker>(1, 0, true));
    score.InsertDynamic(boost::make_shared<Dynamic>(1, 0, 0, Dynamic::ff,
                                                    Dynamic::ff));
    score.InsertAlternateEnding(boost::make_shared<AlternateEnding>(1, 0, 3));

    RemoveSystem action(&score, 1);

    action.redo();
    REQUIRE(score.GetSystemCount() == 2);
    REQUIRE(score.GetTempoMarkerCount() == 0);
    REQUIRE(score.GetDynamicCount() == 0);
    REQUIRE(score.GetAlternateEndingCount() == 0);

    action.undo();
    REQUIRE(score.GetSystemCount() == 3);
    REQUIRE(score.GetTempoMarkerCount() == 1);
    REQUIRE(score.GetDynamicCount() == 1);
    REQUIRE(score.GetAlternateEndingCount() == 1);

    // The symbols in other systems need to be shifted as well.
    RemoveSystem action2(&score, 0);
    action2.redo();
    REQUIRE(score.GetTempoMarkerCount() == 1);
    REQUIRE(score.GetTempoMarker(0)->GetSystem() == 0);
    REQUIRE(score.GetDynamicCount() == 1);
    REQUIRE(score.GetDynamic(0)->GetSystem() == 0);
    REQUIRE(score.GetAlternateEndingCount() == 1);
    REQUIRE(score.GetAlternateEnding(0)->GetSystem() == 0);

    action2.undo();
    REQUIRE(score.GetTempoMarkerCount() == 1);
    REQUIRE(score.GetTempoMarker(0)->GetSystem() == 1);
    REQUIRE(score.GetDynamicCount() == 1);
    REQUIRE(score.GetDynamic(0)->GetSystem() == 1);
    REQUIRE(score.GetAlternateEndingCount() == 1);
    REQUIRE(score.GetAlternateEnding(0)->GetSystem() == 1);
}
