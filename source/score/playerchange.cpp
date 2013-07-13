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

#include "playerchange.h"

#include <algorithm>

ActivePlayer::ActivePlayer()
    : myPlayerNumber(-1),
      myInstrumentNumber(-1)
{
}

ActivePlayer::ActivePlayer(int player, int instrument)
    : myPlayerNumber(player),
      myInstrumentNumber(instrument)
{
}

bool ActivePlayer::operator==(const ActivePlayer &other) const
{
    return myPlayerNumber == other.myPlayerNumber &&
           myInstrumentNumber == other.myInstrumentNumber;
}

int ActivePlayer::getPlayerNumber() const
{
    return myPlayerNumber;
}

int ActivePlayer::getInstrumentNumber() const
{
    return myInstrumentNumber;
}

PlayerChange::PlayerChange()
    : myPosition(0)
{
}

PlayerChange::PlayerChange(int position)
    : myPosition(position)
{
}

bool PlayerChange::operator==(const PlayerChange &other) const
{
    return myPosition == other.myPosition &&
           myActivePlayers == other.myActivePlayers;
}

int PlayerChange::getPosition() const
{
    return myPosition;
}

void PlayerChange::setPosition(int position)
{
    myPosition = position;
}

std::vector<ActivePlayer> PlayerChange::getActivePlayers(int staff) const
{
    if (myActivePlayers.find(staff) != myActivePlayers.end())
        return myActivePlayers.find(staff)->second;
    else
        return std::vector<ActivePlayer>();
}

void PlayerChange::insertActivePlayer(int staff, const ActivePlayer &player)
{
    myActivePlayers[staff].push_back(player);
}

void PlayerChange::removeActivePlayer(int staff, const ActivePlayer &player)
{
    std::vector<ActivePlayer> &playerList = myActivePlayers[staff];
    playerList.erase(std::remove(playerList.begin(), playerList.end(), player),
                     playerList.end());
}
