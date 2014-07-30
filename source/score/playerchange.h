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

#ifndef SCORE_PLAYERCHANGE_H
#define SCORE_PLAYERCHANGE_H

#include "fileversion.h"
#include <map>
#include <vector>

/// An active player is a player that has an instrument assigned to them,
/// and is also assigned to a staff.
class ActivePlayer
{
public:
    ActivePlayer();
    ActivePlayer(int player, int instrument);

    bool operator==(const ActivePlayer &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the zero-based identifier of the player.
    int getPlayerNumber() const;
    /// Returns the zero-based identifier of the instrument.
    int getInstrumentNumber() const;

private:
    int myPlayerNumber;
    int myInstrumentNumber;
};

class PlayerChange
{
public:
    PlayerChange();
    explicit PlayerChange(int position);

    bool operator==(const PlayerChange &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the position within the system where the change is anchored.
    int getPosition() const;
    /// Sets the position within the system where the change is anchored.
    void setPosition(int position);

    /// Returns the set of active players in the given staff.
    std::vector<ActivePlayer> getActivePlayers(int staff) const;

    /// Adds a new active player to a staff.
    void insertActivePlayer(int staff, const ActivePlayer &player);
    /// Removes an active player from a staff.
    void removeActivePlayer(int staff, const ActivePlayer &player);

private:
    int myPosition;
    /// For each staff, there can be multiple active players (or none).
    std::map< int, std::vector<ActivePlayer> > myActivePlayers;
};

template <class Archive>
void ActivePlayer::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("player", myPlayerNumber);
    ar("instrument", myInstrumentNumber);
}

template <class Archive>
void PlayerChange::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("position", myPosition);
    ar("active_players", myActivePlayers);
}

#endif
