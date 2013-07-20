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
#include <powertabdocument/note.h>

EditRest::EditRest(Position* position, const System::StaffPtr staff, const uint32_t insertPos, const uint32_t voice, const uint8_t duration) :
    position(position),
    staff(staff),
    voice(voice),
    newDuration(duration),
    originalDuration(getOriginalDuration(position, duration)),
    wasAlreadyRest(getWasAlreadyRest(position)),
    allocatePosition(position == NULL),
    deletePosition(false)
{
    if (allocatePosition)
    {
        this->position = new Position(insertPos, Position::DEFAULT_DURATION_TYPE, 0);
    }

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

EditRest::~EditRest()
{
    qDeleteAll(notes);

    if (deletePosition)
    {
        delete position;
    }
}

void EditRest::redo()
{
    if (allocatePosition)
    {
        staff->InsertPosition(voice, position);
    }

    if (wasAlreadyRest && (newDuration == originalDuration))
    {
        position->SetRest(false);
        staff->RemovePosition(voice, position->GetPosition());
        deletePosition = true;
    }
    else
    {
        position->SetRest(true);
        position->SetDurationType(newDuration);
        deletePosition = false;
    }

    saveOrRestoreNotes(!wasAlreadyRest);
}

void EditRest::undo()
{
    position->SetRest(wasAlreadyRest);
    position->SetDurationType(originalDuration);
    saveOrRestoreNotes(wasAlreadyRest);

    deletePosition = false;
    if (wasAlreadyRest && (newDuration == originalDuration))
    {
        staff->InsertPosition(voice, position);
    }
    else if (allocatePosition)
    {
        staff->RemovePosition(voice, position->GetPosition());
        deletePosition = true;
    }
}

void EditRest::saveOrRestoreNotes(bool saveNotes)
{
    if (saveNotes)
    {
        assert(notes.empty());

        for (size_t i = 0; i < position->GetNoteCount(); ++i)
        {
            notes.push_back(position->GetNote(i));
        }

        while (position->GetNoteCount())
        {
            position->RemoveNoteByIndex(0);
        }
    }
    else
    {
        assert(position->GetNoteCount() == 0);
        for (size_t i = 0; i < notes.size(); ++i)
        {
            position->InsertNote(notes[i]);
        }
        notes.clear();
    }
}

uint8_t EditRest::getOriginalDuration(const Position* position, const uint8_t duration)
{
    return (position == NULL ? duration : position->GetDurationType());
}

bool EditRest::getWasAlreadyRest(const Position* position)
{
    return (position != NULL && position->IsRest());
}

