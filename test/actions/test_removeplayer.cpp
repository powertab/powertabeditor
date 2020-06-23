/*
  * Copyright (C) 2014 Cameron White
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

#include <actions/removeplayer.h>
#include <score/score.h>

TEST_CASE("Actions/RemovePlayer")
{
    Score score;
    Player player1;
    player1.setDescription("Player 1");
    Player player2;
    player2.setDescription("Player 2");

    PlayerChange change;
    change.insertActivePlayer(0, ActivePlayer(0, 0));
    change.insertActivePlayer(0, ActivePlayer(1, 0));
    System system;
    system.insertPlayerChange(change);
    system.insertStaff(Staff());

    score.insertSystem(system);
    score.insertPlayer(player1);
    score.insertPlayer(player2);

    RemovePlayer action(score, 0);

    action.redo();
    REQUIRE(score.getPlayers().size() == 1);
    REQUIRE(score.getPlayers()[0] == player2);
    {
        const PlayerChange &newChange =
            score.getSystems()[0].getPlayerChanges()[0];
        REQUIRE(newChange.getActivePlayers(0).size() == 1);
        REQUIRE(newChange.getActivePlayers(0)[0].getPlayerNumber() == 0);
    }

    action.undo();
    REQUIRE(score.getPlayers().size() == 2);
    REQUIRE(score.getPlayers()[0] == player1);
    REQUIRE(score.getPlayers()[1] == player2);
    {
        const PlayerChange &newChange =
            score.getSystems()[0].getPlayerChanges()[0];
        REQUIRE(newChange.getActivePlayers(0).size() == 2);
        REQUIRE(newChange.getActivePlayers(0)[0].getPlayerNumber() == 0);
        REQUIRE(newChange.getActivePlayers(0)[1].getPlayerNumber() == 1);
    }
}
