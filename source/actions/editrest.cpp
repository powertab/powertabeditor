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
  
#include "editrest.h"

#include <powertabdocument/position.h>

EditRest::EditRest(Position* position, uint8_t duration) :
    position(position),
    newDuration(duration),
    originalDuration(position->GetDurationType()),
    wasAlreadyRest(position->IsRest())
{
    if (!wasAlreadyRest)
    {
        setText(QObject::tr("Add Rest"));
    }
    else if (originalDuration != newDuration)
    {
        setText(QObject::tr("Edit Rest Duration"));
    }
    else
    {
        setText(QObject::tr("Remove Rest"));
    }
}

void EditRest::redo()
{
    if (wasAlreadyRest && (newDuration == originalDuration))
    {
        position->SetRest(false);
    }
    else
    {
        position->SetRest(true);
        position->SetDurationType(newDuration);
    }
}

void EditRest::undo()
{
    position->SetRest(wasAlreadyRest);
    position->SetDurationType(originalDuration);
}
