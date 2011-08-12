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
  
#include "addtrill.h"

#include <powertabdocument/note.h>

AddTrill::AddTrill(Note* note, quint8 trillFret) :
    note(note),
    trillFret(trillFret)
{
    Q_ASSERT(!note->HasTrill()); // should be editing a note without any current trill

    setText(QObject::tr("Add Trill"));
}

void AddTrill::redo()
{
    note->SetTrill(trillFret);
}

void AddTrill::undo()
{
    note->ClearTrill();
}
