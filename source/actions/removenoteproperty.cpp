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

#include "removenoteproperty.h"

RemoveNoteProperty::RemoveNoteProperty(const ScoreLocation &location,
                                       Note::SimpleProperty property,
                                       const QString &description)
    : QUndoCommand(tr("Remove %1").arg(description)),
      myLocation(location),
      myProperty(property)
{
}

void RemoveNoteProperty::redo()
{
    for (Note *note : myLocation.getSelectedNotes())
        note->setProperty(myProperty, false);
}

void RemoveNoteProperty::undo()
{
    for (Note *note : myLocation.getSelectedNotes())
        note->setProperty(myProperty, true);
}
