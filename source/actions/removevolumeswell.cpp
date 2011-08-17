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

#include "removevolumeswell.h"

#include <powertabdocument/position.h>

RemoveVolumeSwell::RemoveVolumeSwell(Position* position) :
    position(position)
{
    Q_ASSERT(position->HasVolumeSwell());
    position->GetVolumeSwell(originalStartVolume, originalEndVolume,
                             originalDuration);

    setText(QObject::tr("Remove Volume Swell"));
}

void RemoveVolumeSwell::redo()
{
    position->ClearVolumeSwell();
}

void RemoveVolumeSwell::undo()
{
    position->SetVolumeSwell(originalStartVolume, originalEndVolume,
                             originalDuration);
}
