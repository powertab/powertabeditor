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

#include <actions/deleteposition.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

DeleteNote::DeleteNote(boost::shared_ptr<Staff> staff, uint32_t voice,
                       Position *position, uint8_t string) :
    staff(staff), voice(voice),
    positionIndex(position->GetPosition()),
    string(string), note(position->GetNoteByString(string))
{
    if (position->GetNoteCount() == 1)
    {
        deletePosition.reset(new DeletePosition(staff, position, voice));
    }

    setText(QObject::tr("Clear Note"));
}

DeleteNote::~DeleteNote()
{
}

void DeleteNote::redo()
{
    if (deletePosition)
    {
        deletePosition->redo();
    }
    else
    {
        staff->GetPositionByPosition(voice, positionIndex)->RemoveNote(string);
    }
}

void DeleteNote::undo()
{
    if (deletePosition)
    {
        deletePosition->undo();
    }
    else
    {
        staff->GetPositionByPosition(voice, positionIndex)->InsertNote(note);
    }
}

bool DeleteNote::canExecute(Position *position, uint8_t string)
{
    return position && position->GetNoteByString(string);
}

