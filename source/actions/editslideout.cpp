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
  
#include "editslideout.h"

#include <powertabdocument/note.h>

EditSlideOut::EditSlideOut(Note* note, quint8 slideType, qint8 steps) :
    note(note),
    newSlideType(slideType),
    newSteps(steps)
{
    Q_ASSERT(Note::IsValidSlideOutOfType(slideType));

    QString undoText;

    if (slideType == Note::slideOutOfShiftSlide)
        undoText = QObject::tr("Shift Slide");
    else if (slideType == Note::slideOutOfLegatoSlide)
        undoText = QObject::tr("Legato Slide");
    else if (slideType == Note::slideOutOfDownwards)
        undoText = QObject::tr("Slide Out Of Downwards");
    else if (slideType == Note::slideOutOfUpwards)
        undoText = QObject::tr("Slide Out Of Upwards");
    else if (slideType == Note::slideOutOfNone)
        undoText = QObject::tr("Remove Slide");

    setText(undoText);

    note->GetSlideOutOf(oldSlideType, oldSteps);
}

void EditSlideOut::redo()
{
    note->SetSlideOutOf(newSlideType, newSteps);
}

void EditSlideOut::undo()
{
    note->SetSlideOutOf(oldSlideType, oldSteps);
}
