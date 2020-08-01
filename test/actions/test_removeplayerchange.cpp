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
  
#include <doctest/doctest.h>

#include <actions/removeplayerchange.h>
#include <score/score.h>
#include <score/scorelocation.h>

TEST_CASE("Actions/RemovePlayerChange")
{
    Score score;
    System system;
    PlayerChange change(5);
    change.insertActivePlayer(1, ActivePlayer(0, 2));
    system.insertPlayerChange(change);
    score.insertSystem(system);

    RemovePlayerChange action(ScoreLocation(score, 0, 0, 5));

    action.redo();
    REQUIRE(score.getSystems()[0].getPlayerChanges().size() == 0);

    action.undo();
    REQUIRE(score.getSystems()[0].getPlayerChanges().size() == 1);
    REQUIRE(score.getSystems()[0].getPlayerChanges()[0] == change);
}
