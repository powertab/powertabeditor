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

#include "addvolumeswell.h"

#include <powertabdocument/position.h>

AddVolumeSwell::AddVolumeSwell(Position* position, uint8_t startVol,
                               uint8_t endVol, uint8_t duration) :
    position(position), startVol(startVol),
    endVol(endVol), duration(duration)
{
    Q_ASSERT(!position->HasVolumeSwell());

    setText(QObject::tr("Add Volume Swell"));
}

void AddVolumeSwell::redo()
{
    position->SetVolumeSwell(startVol, endVol, duration);
}

void AddVolumeSwell::undo()
{
    position->ClearVolumeSwell();
}
