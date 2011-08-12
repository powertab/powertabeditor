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
  
#include "editslideinto.h"

#include <powertabdocument/note.h>

EditSlideInto::EditSlideInto(Note* note, uint8_t newSlideIntoType) :
    note(note),
    newSlideIntoType(newSlideIntoType)
{
    note->GetSlideInto(oldSlideIntoType);

    switch(newSlideIntoType)
    {
    case Note::slideIntoNone:
        setText(QObject::tr("Remove Slide Into"));
        break;

    case Note::slideIntoFromBelow:
        setText(QObject::tr("Slide Into From Below"));
        break;

    case Note::slideIntoFromAbove:
        setText(QObject::tr("Slide Into From Above"));
        break;

    default:
        Q_ASSERT(false); // invalid/unsupported slide type
        break;
    }
}

void EditSlideInto::redo()
{
    note->SetSlideInto(newSlideIntoType);
}

void EditSlideInto::undo()
{
    note->SetSlideInto(oldSlideIntoType);
}
