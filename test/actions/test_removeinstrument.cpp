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
  
#include <catch.hpp>

#include <actions/removeinstrument.h>
#include <score/score.h>

TEST_CASE("Actions/RemoveInstrument", "")
{
    Score score;
    Instrument instrument1;
    instrument1.setDescription("Instrument 1");
    Instrument instrument2;
    instrument2.setDescription("Instrument 2");

    PlayerChange change;
    change.insertActivePlayer(0, ActivePlayer(0, 0));
    change.insertActivePlayer(0, ActivePlayer(0, 1));
    System system;
    system.insertPlayerChange(change);
    system.insertStaff(Staff());

    score.insertSystem(system);
    score.insertInstrument(instrument1);
    score.insertInstrument(instrument2);

    RemoveInstrument action(score, 0);

    action.redo();
    REQUIRE(score.getInstruments().size() == 1);
    REQUIRE(score.getInstruments()[0] == instrument2);
    {
        const PlayerChange &newChange =
            score.getSystems()[0].getPlayerChanges()[0];
        REQUIRE(newChange.getActivePlayers(0).size() == 1);
        REQUIRE(newChange.getActivePlayers(0)[0].getInstrumentNumber() == 0);
    }

    action.undo();
    REQUIRE(score.getInstruments().size() == 2);
    REQUIRE(score.getInstruments()[0] == instrument1);
    REQUIRE(score.getInstruments()[1] == instrument2);
    {
        const PlayerChange &newChange =
            score.getSystems()[0].getPlayerChanges()[0];
        REQUIRE(newChange.getActivePlayers(0).size() == 2);
        REQUIRE(newChange.getActivePlayers(0)[0].getInstrumentNumber() == 0);
        REQUIRE(newChange.getActivePlayers(0)[1].getInstrumentNumber() == 1);
    }
}
