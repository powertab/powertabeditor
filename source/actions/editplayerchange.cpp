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

#include "editplayerchange.h"

#include <score/system.h>
#include <score/utils.h>

AddPlayerChange::AddPlayerChange(const ScoreLocation &location,
                                 const PlayerChange &change)
    : QUndoCommand(tr("Add Player Change")),
      myLocation(location),
      myPlayerChange(change)
{
    myPlayerChange.setPosition(location.getPositionIndex());
}

void
AddPlayerChange::redo()
{
    myLocation.getSystem().insertPlayerChange(myPlayerChange);
}

void
AddPlayerChange::undo()
{
    myLocation.getSystem().removePlayerChange(myPlayerChange);
}

RemovePlayerChange::RemovePlayerChange(const ScoreLocation &location)
    : QUndoCommand(tr("Remove Player Change")),
      myLocation(location),
      myPlayerChange(*ScoreUtils::findByPosition(
          location.getSystem().getPlayerChanges(), location.getPositionIndex()))
{
}

void
RemovePlayerChange::redo()
{
    myLocation.getSystem().removePlayerChange(myPlayerChange);
}

void
RemovePlayerChange::undo()
{
    myLocation.getSystem().insertPlayerChange(myPlayerChange);
}
