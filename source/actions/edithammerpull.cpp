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
  
#include "edithammerpull.h"

#include <powertabdocument/note.h>

EditHammerPull::EditHammerPull(Note* note, HammerPullFlags hammer) :
    note(note),
    hammer(hammer),
    set(set)
{
    QString undoText;

    if (hammer == hammerOn)
        undoText = QObject::tr(note->HasHammerOn() ? "Remove " : "Set " ) + QObject::tr("Hammer On");
      else
        undoText = QObject::tr(note->HasPullOff() ? "Remove " : "Set " ) + QObject::tr("Pull Off");
    
    setText(undoText);

    note->GetSlideOutOf(oldSlideType, oldSteps);
}

void EditHammerPull::redo()
{
    if (note->HasSlideOutOf())
        note->SetSlideOutOf(Note::slideOutOfNone, 0);

    if (hammer == hammerOn)
      note->SetHammerOn(!note->HasHammerOn());
    else
      note->SetPullOff(!note->HasPullOff());
}

void EditHammerPull::undo()
{
    if (hammer == hammerOn)
      note->SetHammerOn(!note->HasHammerOn());
    else
      note->SetPullOff(!note->HasPullOff());

    note->SetSlideOutOf(oldSlideType, oldSteps);
}
