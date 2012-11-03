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

#include <boost/make_shared.hpp>

#include <powertabdocument/score.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/dynamic.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/systemlocation.h>

TEST_CASE("PowerTabDocument/Score/CopyAndEquality", "")
{
    Score score1("Guitar"), score2("Guitar");

    // basic checks for equality
    REQUIRE(score1 == score2);
    score1.InsertAlternateEnding(Score::AlternateEndingPtr(new AlternateEnding));
    REQUIRE(score1 != score2);

    // check for deep copy and equality
    Score score3(score1);
    REQUIRE(score3 == score1);
    score3.GetAlternateEnding(0)->SetDalSegno();
    REQUIRE(*score3.GetAlternateEnding(0) != *score1.GetAlternateEnding(0));
}

TEST_CASE("PowerTabDocument/Score/InsertTempoMarker", "")
{
    Score score("Guitar");

    REQUIRE(score.GetTempoMarkerCount() == 0u);

    score.InsertTempoMarker(Score::TempoMarkerPtr(new TempoMarker(0, 0, false)));

    REQUIRE(score.GetTempoMarkerCount() == 1u);
}

TEST_CASE("PowerTabDocument/Score/Dynamics", "")
{
    Score score("Guitar");
    REQUIRE(score.GetDynamicCount() == 0u);

    boost::shared_ptr<Dynamic> dynamic = boost::make_shared<Dynamic>(1, 2, 3, Dynamic::mp, Dynamic::notSet);
    score.InsertDynamic(dynamic);

    REQUIRE(score.GetDynamicCount() == 1u);
    REQUIRE(score.FindDynamic(1, 2, 3) == dynamic);

    score.RemoveDynamic(dynamic);
    REQUIRE(score.GetDynamicCount() == 0u);
}

TEST_CASE("PowerTabDocument/Score/TempoMarkers", "")
{
    Score score("Guitar");
    REQUIRE(score.GetTempoMarkerCount() == 0u);

    boost::shared_ptr<TempoMarker> marker = boost::make_shared<TempoMarker>();
    marker->SetSystem(1);
    marker->SetPosition(2);
    score.InsertTempoMarker(marker);

    REQUIRE(score.GetTempoMarkerCount() == 1u);
    REQUIRE(score.FindTempoMarker(SystemLocation(1, 2)) == marker);

    score.RemoveTempoMarker(marker);
    REQUIRE(score.GetTempoMarkerCount() == 0u);
}
