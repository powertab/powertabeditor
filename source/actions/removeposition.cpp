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

#include "removeposition.h"

#include <score/staff.h>

RemovePosition::RemovePosition(const ScoreLocation &location,
                               const QString &text)
    : QUndoCommand(text),
      myLocation(location),
      myOriginalPosition(*location.getPosition())
{
}

void RemovePosition::redo()
{
    myLocation.getVoice().removePosition(myOriginalPosition);
}

void RemovePosition::undo()
{
    myLocation.getVoice().insertPosition(myOriginalPosition);
}
