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
  
#include "positionshift.h"

#include <powertabdocument/system.h>

using std::shared_ptr;

PositionShift::PositionShift(shared_ptr<System> system, quint32 positionIndex, ShiftType type) :
    system(system),
    positionIndex(positionIndex),
    type(type)
{
    if (type == SHIFT_FORWARD)
    {
        setText(QObject::tr("Shift Forward"));
    }
    else
    {
        setText(QObject::tr("Shift Backward"));
    }
}

void PositionShift::redo()
{
    if (type == SHIFT_FORWARD)
    {
        system->ShiftForward(positionIndex);
    }
    else
    {
        system->ShiftBackward(positionIndex);
    }
}

void PositionShift::undo()
{
    if (type == SHIFT_FORWARD)
    {
        system->ShiftBackward(positionIndex);
    }
    else
    {
        system->ShiftForward(positionIndex);
    }
}
