/*
  * Copyright (C) 2011 Cameron White
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
  
#include "addnote.h"

#include <score/staff.h>

AddNote::AddNote(const ScoreLocation &location, const Note &note,
                 Position::DurationType duration)
    : QUndoCommand(tr("Add Note")),
      myLocation(location),
      myNote(note),
      myDuration(duration)
{
    if (myLocation.getPosition())
        myOriginalPosition = *myLocation.getPosition();
}

void AddNote::redo()
{
    Position *pos = myLocation.getPosition();

    // Add a new position if necessary.
    if (!pos)
    {
        myLocation.getVoice().insertPosition(
            Position(myLocation.getPositionIndex(), myDuration));
        pos = myLocation.getPosition();
    }

    pos->setRest(false);
    pos->insertNote(myNote);
}

void AddNote::undo()
{
    if (!myOriginalPosition)
        myLocation.getVoice().removePosition(*myLocation.getPosition());
    else
        *myLocation.getPosition() = *myOriginalPosition;
}
