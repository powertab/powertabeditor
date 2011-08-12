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
  
#include "shifttabnumber.h"

#include <painters/caret.h>

#include <powertabdocument/note.h>

ShiftTabNumber::ShiftTabNumber(Caret* caret, Position* currentPos, Note* note, Position::ShiftType direction, quint8 numStringsInStaff, const Tuning& tuning) :
    caret(caret),
    currentPos(currentPos),
    note(note),
    direction(direction),
    numStringsInStaff(numStringsInStaff),
    tuning(tuning)
{
    if (direction == Position::SHIFT_UP)
    {
        setText(QObject::tr("Shift Tab Number Up"));
    }
    else
    {
        setText(QObject::tr("Shift Tab Number Down"));
    }
}

void ShiftTabNumber::redo()
{
    currentPos->ShiftTabNumber(note, direction, numStringsInStaff, tuning);
    caret->setCurrentStringIndex(note->GetString());
}

void ShiftTabNumber::undo()
{
    const Position::ShiftType oppositeDirection = (direction == Position::SHIFT_UP) ? Position::SHIFT_DOWN : Position::SHIFT_UP;
    currentPos->ShiftTabNumber(note, oppositeDirection, numStringsInStaff, tuning);
    caret->setCurrentStringIndex(note->GetString());
}
