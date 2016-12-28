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

#include "addrest.h"

#include <score/voice.h>

AddRest::AddRest(const ScoreLocation &location, Position::DurationType duration)
    : QUndoCommand(QObject::tr("Add Rest")),
      myLocation(location),
      myDuration(duration)
{
    if (myLocation.getPosition())
        myOriginalPosition = *myLocation.getPosition();
}

void AddRest::redo()
{
    Position *pos = myLocation.getPosition();

    // Add a new position if necessary.
    if (!pos)
    {
        myLocation.getVoice().insertPosition(
            Position(myLocation.getPositionIndex(), myDuration));
        pos = myLocation.getPosition();
    }

    pos->setDurationType(myDuration);
    pos->setRest(true);
}

void AddRest::undo()
{
    if (!myOriginalPosition)
        myLocation.getVoice().removePosition(*myLocation.getPosition());
    else
        *myLocation.getPosition() = *myOriginalPosition;
}
