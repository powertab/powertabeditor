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
  
#include "updatetabnumber.h"
#include <powertabdocument/staff.h>

UpdateTabNumber::UpdateTabNumber(uint8_t typedNumber, Note* note,
                                 Position* position, uint32_t voice,
                                 boost::shared_ptr<Staff> staff) :
    voice(voice),
    note(note),
    position(position),
    staff(staff),
    origPrevNote(NULL),
    origNextNote(NULL)
{
    // save old fret number so we can undo
    prevFretNumber = note->GetFretNumber();
    if (prevFretNumber == 1 || prevFretNumber == 2)
    {
        // if old fret number was a 1 or 2 then we are typing a double digit number
        newFretNumber = prevFretNumber * 10 + typedNumber;
    }
    else
    {
        newFretNumber = typedNumber;
    }

    Note *tempNote = staff->GetAdjacentNoteOnString(Staff::PrevNote, position,
                                                    note, voice);
    if (tempNote)
        origPrevNote = tempNote->CloneObject();

    tempNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note,
                                              voice);
    if (tempNote)
        origNextNote = tempNote->CloneObject();

    origNote = note->CloneObject();

    setText(QObject::tr("Update Tab Number"));
}

UpdateTabNumber::~UpdateTabNumber()
{
    delete origNote;
    delete origNextNote;
    delete origPrevNote;
}

void UpdateTabNumber::redo()
{
    staff->UpdateTabNumber(position, note, voice, newFretNumber);
}

void UpdateTabNumber::undo()
{
    *note = *origNote;

    if (origPrevNote)
    {
        Note *prevNote = staff->GetAdjacentNoteOnString(Staff::PrevNote,
                                                        position, note, voice);
        *prevNote = *origPrevNote;
    }

    if (origNextNote)
    {
        Note *nextNote = staff->GetAdjacentNoteOnString(Staff::NextNote,
                                                        position, note, voice);
        *nextNote = *origNextNote;
    }
}
