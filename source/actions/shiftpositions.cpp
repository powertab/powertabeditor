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
  
#include "shiftpositions.h"

#include <score/system.h>

ShiftPositions::ShiftPositions(const ScoreLocation &location,
                             ShiftPositions::ShiftType type)
    : QUndoCommand(type == Forward ? tr("Shift Forward") :
                                     tr("Shift Backward")),
      myLocation(location),
      myShiftType(type)
{
}

void ShiftPositions::redo()
{
    if (myShiftType == Forward)
    {
        SystemUtils::shiftForward(myLocation.getSystem(),
                                  myLocation.getPositionIndex());
    }
    else
    {
        SystemUtils::shiftBackward(myLocation.getSystem(),
                                   myLocation.getPositionIndex());
    }
}

void ShiftPositions::undo()
{
    if (myShiftType == Forward)
    {
        SystemUtils::shiftBackward(myLocation.getSystem(),
                                   myLocation.getPositionIndex());
    }
    else
    {
        SystemUtils::shiftForward(myLocation.getSystem(),
                                  myLocation.getPositionIndex());
    }
}
