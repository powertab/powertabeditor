#include "updatetabnumber.h"

UpdateTabNumber::UpdateTabNumber(uint8_t typedNumber, Note* note, Position* position, Staff* staff) :
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

    Note *tempNote = staff->GetAdjacentNoteOnString(Staff::PrevNote, position, note);
    if (tempNote)
        origPrevNote = tempNote->CloneObject();

    tempNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note);
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
    staff->UpdateTabNumber(position, note, newFretNumber);
}

void UpdateTabNumber::undo()
{
    *note = *origNote;

    if (origPrevNote)
    {
        Note *prevNote = staff->GetAdjacentNoteOnString(Staff::PrevNote, position, note);
        *prevNote = *origPrevNote;
    }

    if (origNextNote)
    {
        Note *nextNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note);
        *nextNote = *origNextNote;
    }
}
