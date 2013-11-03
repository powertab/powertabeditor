/*
  * Copyright (C) 2012 Cameron White
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

#include "removenote.h"

#include <score/staff.h>

RemoveNote::RemoveNote(const ScoreLocation &location)
    : QUndoCommand(QObject::tr("Clear Note")),
      myLocation(location),
      myOriginalPosition(*location.getPosition())
{
    Q_ASSERT(myLocation.getNote());
}

void RemoveNote::redo()
{
    Position *pos = myLocation.getPosition();
    Q_ASSERT(pos);

    pos->removeNote(*myLocation.getNote());

    if (pos->getNotes().empty())
        myLocation.getVoice().removePosition(*pos);
}

void RemoveNote::undo()
{
    if (!myLocation.getPosition())
        myLocation.getVoice().insertPosition(myOriginalPosition);
    else
        *myLocation.getPosition() = myOriginalPosition;
}
