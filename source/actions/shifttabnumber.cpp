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
#include <powertabdocument/staff.h>

ShiftTabNumber::ShiftTabNumber(boost::shared_ptr<Staff> staff,
                               Position* position, Note* note, uint32_t voice,
                               Position::ShiftType direction,
                               const Tuning& tuning) :
    voice(voice),
    staff(staff),
    position(position),
    note(note),
    shiftUp(direction == Position::SHIFT_UP),
    tuning(tuning)
{
    setText(shiftUp ? QObject::tr("Shift Tab Number Up") :
                      QObject::tr("Shift Tab Number Down"));

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

    origNote = *note;
}

void ShiftTabNumber::redo()
{
    staff->ShiftTabNumber(position, note, voice, shiftUp, tuning);
}

void ShiftTabNumber::undo()
{
    *note = origNote;

    if (prevNote)
    {
        *prevNote = origPrevNote;
    }

    if (nextNote)
    {
        *nextNote = origNextNote;
    }
}
