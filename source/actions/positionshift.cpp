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

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

using boost::shared_ptr;

PositionShift::PositionShift(Score* score, shared_ptr<System> system,
                             uint32_t positionIndex, ShiftType type) :
    score(score),
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
        score->ShiftForward(system, positionIndex);
    }
    else
    {
        score->ShiftBackward(system, positionIndex);
    }
}

void PositionShift::undo()
{
    if (type == SHIFT_FORWARD)
    {
        score->ShiftBackward(system, positionIndex);
    }
    else
    {
        score->ShiftForward(system, positionIndex);
    }
}
