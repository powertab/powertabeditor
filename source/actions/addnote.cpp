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
  
#include "addnote.h"

#include <powertabdocument/position.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>

AddNote::AddNote(uint8_t stringNum, uint8_t fretNumber,
                 uint32_t positionIndex, uint32_t voice, boost::shared_ptr<Staff> staff) :
    stringNum(stringNum),
    positionIndex(positionIndex),
    voice(voice),
    staff(staff),
    note(new Note(stringNum, fretNumber)),
    position(staff->GetPositionByPosition(voice, positionIndex)),
    newPositionAdded(false)
{
    setText(QObject::tr("Add Note"));
}

AddNote::~AddNote()
{
}

void AddNote::redo()
{
    if (!staff->GetPositionByPosition(voice, positionIndex)) // add a Position if necessary
    {
        if (!newPositionAdded)
        {
            newPositionAdded = true;
            position = new Position(positionIndex, 8, 0);
        }
        staff->InsertPosition(voice, position);
    }

    position->InsertNote(note);
}

void AddNote::undo()
{
    position->RemoveNote(stringNum);

    if (newPositionAdded)
    {
        staff->RemovePosition(voice, positionIndex);
    }
}
