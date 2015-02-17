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

#include <score/score.h>

TEST_CASE("Score/Score/Systems", "")
{
    Score score;

    REQUIRE(score.getSystems().size() == 0);

    System system;
    score.insertSystem(system);
    REQUIRE(score.getSystems().size() == 1);
    score.removeSystem(0);
    REQUIRE(score.getSystems().size() == 0);
}

TEST_CASE("Score/Score/Players", "")
{
    Score score;

    REQUIRE(score.getPlayers().size() == 0);

    Player player;
    score.insertPlayer(player);
    REQUIRE(score.getPlayers().size() == 1);
    score.removePlayer(0);
    REQUIRE(score.getPlayers().size() == 0);
}

TEST_CASE("Score/Score/Instruments", "")
{
    Score score;

    REQUIRE(score.getInstruments().size() == 0);

    Instrument instrument;
    score.insertInstrument(instrument);
    REQUIRE(score.getInstruments().size() == 1);
    score.removeInstrument(0);
    REQUIRE(score.getInstruments().size() == 0);
}

TEST_CASE("Score/Score/ViewFilters", "")
{
    Score score;

    REQUIRE(score.getViewFilters().size() == 0);

    ViewFilter filter1;
    filter1.addRule(FilterRule(FilterRule::PLAYER_NAME, "foo"));
    score.insertViewFilter(filter1);

    ViewFilter filter2;
    filter2.addRule(FilterRule(FilterRule::PLAYER_NAME, "bar"));
    score.insertViewFilter(filter2);

    REQUIRE(score.getViewFilters().size() == 2);
    score.removeViewFilter(1);
    REQUIRE(score.getViewFilters().size() == 1);
    REQUIRE(score.getViewFilters()[0] == filter1);
}
