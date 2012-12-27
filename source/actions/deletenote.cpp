/*
  * Copyright (C) 2012 Cameron White
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

#include "deletenote.h"

#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

DeleteNote::DeleteNote(boost::shared_ptr<Staff> staff, uint32_t voice,
                       Position *position, uint8_t string,
                       bool clearEmptyPositions) :
    QUndoCommand(QObject::tr("Clear Note")),
    staff(staff), voice(voice), position(position), string(string),
    deletePosition(position->GetNoteCount() == 1 && clearEmptyPositions),
    note(position->GetNoteByString(string))
{
    prevNote = staff->GetAdjacentNoteOnString(Staff::PrevNote, position, note,
                                              voice);
    if (prevNote)
    {
        origPrevNote = *prevNote;
    }

    nextNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note,
                                              voice);
    if (nextNote)
    {
        origNextNote = *nextNote;
    }
}

DeleteNote::~DeleteNote()
{
}

void DeleteNote::redo()
{
    staff->RemoveNote(voice, position->GetPosition(), string);

    if (deletePosition)
    {
        staff->RemovePosition(voice, position->GetPosition());
    }
}

void DeleteNote::undo()
{
    if (deletePosition)
    {
        staff->InsertPosition(voice, position);
    }

    position->InsertNote(note);

    if (prevNote)
    {
        *prevNote = origPrevNote;
    }

    if (nextNote)
    {
        *nextNote = origNextNote;
    }
}

bool DeleteNote::canExecute(Position *position, uint8_t string)
{
    return position && position->GetNoteByString(string);
}
