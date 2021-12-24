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

#include "volumeswell.h"

AddVolumeSwell::AddVolumeSwell(const ScoreLocation &location,
                               const VolumeSwell &swell)
    : QUndoCommand(tr("Add Volume Swell")),
      myLocation(location),
      mySwell(swell)
{
}

void
AddVolumeSwell::redo()
{
    myLocation.getPosition()->setVolumeSwell(mySwell);
}

void
AddVolumeSwell::undo()
{
    myLocation.getPosition()->clearVolumeSwell();
}

RemoveVolumeSwell::RemoveVolumeSwell(const ScoreLocation &location)
    : QUndoCommand(tr("Remove Volume Swell")),
      myLocation(location),
      mySwell(location.getPosition()->getVolumeSwell())
{
}

void
RemoveVolumeSwell::redo()
{
    myLocation.getPosition()->clearVolumeSwell();
}

void
RemoveVolumeSwell::undo()
{
    myLocation.getPosition()->setVolumeSwell(mySwell);
}
