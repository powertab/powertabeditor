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
#include "test_serialization.h"

TEST_CASE("Score/Score/Systems", "")
{
    Score::Score score;

    REQUIRE(score.getSystems().size() == 0);

    Score::System system;
    score.insertSystem(system);
    REQUIRE(score.getSystems().size() == 1);
    score.removeSystem(system);
    REQUIRE(score.getSystems().size() == 0);
}

TEST_CASE("Score/Score/Players", "")
{
    Score::Score score;

    REQUIRE(score.getPlayers().size() == 0);

    Score::Player player;
    score.insertPlayer(player);
    REQUIRE(score.getPlayers().size() == 1);
    score.removePlayer(player);
    REQUIRE(score.getPlayers().size() == 0);
}

TEST_CASE("Score/Score/Instruments", "")
{
    Score::Score score;

    REQUIRE(score.getInstruments().size() == 0);

    Score::Instrument instrument;
    score.insertInstrument(instrument);
    REQUIRE(score.getInstruments().size() == 1);
    score.removeInstrument(instrument);
    REQUIRE(score.getInstruments().size() == 0);
}
