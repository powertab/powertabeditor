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
  
#include "removealternateending.h"

#include <score/system.h>
#include <score/utils.h>

RemoveAlternateEnding::RemoveAlternateEnding(const ScoreLocation &location)
    : QUndoCommand(tr("Remove Repeat Ending")),
      myLocation(location),
      myOriginalEnding(*ScoreUtils::findByPosition(
                           location.getSystem().getAlternateEndings(),
                           location.getPositionIndex()))
{
}

void RemoveAlternateEnding::redo()
{
    myLocation.getSystem().removeAlternateEnding(myOriginalEnding);
}

void RemoveAlternateEnding::undo()
{
    myLocation.getSystem().insertAlternateEnding(myOriginalEnding);
}
