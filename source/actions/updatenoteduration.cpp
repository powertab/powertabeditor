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
  
#include "updatenoteduration.h"

#include <powertabdocument/position.h>

UpdateNoteDuration::UpdateNoteDuration(Position* position, quint8 duration) :
    position(position),
    newDuration(duration)
{
    setText(QObject::tr("Update Note Duration"));
    originalDuration = position->GetDurationType();

    Q_ASSERT(position != NULL);
    Q_ASSERT(position->IsValidDurationType(newDuration));
}

void UpdateNoteDuration::redo()
{
    position->SetDurationType(newDuration);
}

void UpdateNoteDuration::undo()
{
    position->SetDurationType(originalDuration);
}
