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

#include <score/playerchange.h>
#include "test_serialization.h"

TEST_CASE("Score/PlayerChange/ActivePlayers")
{
    PlayerChange change;
    ActivePlayer player(3, 2);

    change.insertActivePlayer(1, player);
    REQUIRE(change.getActivePlayers(0).empty());
    REQUIRE(change.getActivePlayers(1).size() == 1);

    change.removeActivePlayer(1, ActivePlayer(3, 2));
    REQUIRE(change.getActivePlayers(1).empty());
}

TEST_CASE("Score/PlayerChange/Serialization")
{
    PlayerChange change;
    change.setPosition(42);
    change.insertActivePlayer(1, ActivePlayer(3, 2));
	change.insertActivePlayer(1, ActivePlayer(1, 2));
	change.insertActivePlayer(2, ActivePlayer(4, 3));

    Serialization::test("player_change", change);
}
