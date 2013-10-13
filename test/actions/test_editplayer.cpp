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

#include <actions/editplayer.h>
#include <score/score.h>

TEST_CASE("Actions/EditPlayer", "")
{
    Score score;
    Player oldPlayer;
    oldPlayer.setDescription("Description 1");
    oldPlayer.setMaxVolume(90);

    score.insertPlayer(oldPlayer);

    Player newPlayer(oldPlayer);
    newPlayer.setDescription("Description 2");

    EditPlayer action(score, 0, newPlayer);

    action.redo();
    REQUIRE(score.getPlayers()[0] == newPlayer);

    action.undo();
    REQUIRE(score.getPlayers()[0] == oldPlayer);
}
