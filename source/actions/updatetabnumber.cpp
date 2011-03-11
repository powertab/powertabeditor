#include "updatetabnumber.h"

#include <painters/caret.h>
#include <iostream>

UpdateTabNumber::UpdateTabNumber(uint8_t typedNumber, Note* note, Position* position, Staff* staff) :
    note(note),
    position(position),
    staff(staff)
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

    setText(QObject::tr("Update Tab Number"));
}

void UpdateTabNumber::redo()
{
    staff->UpdateTabNumber(position, note, newFretNumber);
}

void UpdateTabNumber::undo()
{
    staff->UpdateTabNumber(position, note, prevFretNumber);
}
