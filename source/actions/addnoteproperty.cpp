/*
  * Copyright (C) 2013 Cameron White
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

#include "addnoteproperty.h"

AddNoteProperty::AddNoteProperty(const ScoreLocation &location,
                                 Note::SimpleProperty property,
                                 const QString &description)
    : QUndoCommand(tr("Set %1").arg(description)),
      myLocation(location),
      myProperty(property)
{
    for (const Note *note : myLocation.getSelectedNotes())
        myOriginalNotes.push_back(*note);
}

void AddNoteProperty::redo()
{
    for (Note *note : myLocation.getSelectedNotes())
        note->setProperty(myProperty, true);
}

void AddNoteProperty::undo()
{
    std::vector<Note *> selectedNotes = myLocation.getSelectedNotes();

    for (size_t i = 0; i < myOriginalNotes.size(); ++i)
        *selectedNotes[i] = myOriginalNotes[i];
}
