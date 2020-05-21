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

TEST_CASE("Actions/EditTuning", "")
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

    // Change the tuning.
    Player newPlayer(player1);
    Tuning tuning(newPlayer.getTuning());
    auto notes = tuning.getNotes();
    notes.pop_back();
    tuning.setNotes(notes);
    newPlayer.setTuning(tuning);

    EditPlayer action(score, 0, newPlayer);

    action.redo();
    const PlayerChange &newChange = score.getSystems()[0].getPlayerChanges()[0];
    REQUIRE(newChange.getActivePlayers(0).size() == 1);
    REQUIRE(newChange.getActivePlayers(0)[0].getPlayerNumber() == 1);

    action.undo();
    const PlayerChange &reverted = score.getSystems()[0].getPlayerChanges()[0];
    REQUIRE(reverted.getActivePlayers(0).size() == 2);
}
