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
  
#include "deleteposition.h"

#include <boost/foreach.hpp>

#include <actions/deletenote.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>

DeletePosition::DeletePosition(boost::shared_ptr<Staff> staff,
                               Position* position, quint32 voice) :
    QUndoCommand(QObject::tr("Clear Position")),
    staff(staff), position(position), voice(voice)
{
    for (size_t i = 0; i < position->GetNoteCount(); ++i)
    {
        deleteNotes.push_back(new DeleteNote(staff, voice, position,
                position->GetNote(i)->GetString(), false));
    }
}

void DeletePosition::redo()
{
    // Remove each note first, so that e.g. hammerons of adjacent notes are
    // updated properly.
    BOOST_FOREACH(QUndoCommand& cmd, deleteNotes)
    {
        cmd.redo();
    }

    staff->RemovePosition(voice, position->GetPosition());
}

void DeletePosition::undo()
{
    staff->InsertPosition(voice, position);

    BOOST_FOREACH(QUndoCommand& cmd, deleteNotes)
    {
        cmd.undo();
    }
}
