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
  
#include <catch2/catch.hpp>

#include <actions/addplayer.h>
#include <score/score.h>

TEST_CASE("Actions/AddPlayer", "")
{
    Score score;
    Player player;
    player.setDescription("Test Player");

    AddPlayer action(score, player);

    action.redo();
    REQUIRE(score.getPlayers().size() == 1);
    REQUIRE(score.getPlayers()[0] == player);

    action.undo();
    REQUIRE(score.getPlayers().size() == 0);
}
