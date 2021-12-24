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
  
#include "removedynamic.h"

#include <score/staff.h>
#include <score/utils.h>

RemoveDynamic::RemoveDynamic(const ScoreLocation &location)
    : QUndoCommand(tr("Remove Dynamic")),
      myLocation(location),
      myOriginalDynamic(*ScoreUtils::findByPosition(
                            location.getStaff().getDynamics(),
                            location.getPositionIndex()))
{
}

void RemoveDynamic::redo()
{
    myLocation.getStaff().removeDynamic(myOriginalDynamic);
}

void RemoveDynamic::undo()
{
    myLocation.getStaff().insertDynamic(myOriginalDynamic);
}
