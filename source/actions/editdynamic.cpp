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

#include "editdynamic.h"

#include <score/staff.h>

EditDynamic::EditDynamic(const ScoreLocation &location,
                         const Dynamic &originalDynamic,
                         const Dynamic &newDynamic)
    : QUndoCommand(tr("Edit Dynamic")),
      myLocation(location),
      myOriginalDynamic(originalDynamic),
      myNewDynamic(newDynamic)
{
}

void EditDynamic::redo()
{
    myLocation.getStaff().removeDynamic(myOriginalDynamic);
    myLocation.getStaff().insertDynamic(myNewDynamic);
}

void EditDynamic::undo()
{
    myLocation.getStaff().removeDynamic(myNewDynamic);
    myLocation.getStaff().insertDynamic(myOriginalDynamic);
}
