#include "addnote.h"

#include <powertabdocument/position.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>

AddNote::AddNote(uint8_t stringNum, uint8_t fretNumber,
                 uint32_t positionIndex, uint32_t voice, std::shared_ptr<Staff> staff) :
    stringNum(stringNum),
    fretNumber(fretNumber),
    positionIndex(positionIndex),
    voice(voice),
    staff(staff),
    newPositionAdded(false),
    position(NULL)
{
    setText(QObject::tr("Add Note"));
}

AddNote::~AddNote()
{
}

void AddNote::redo()
{
    position = staff->GetPositionByPosition(voice, positionIndex);

    if (!position) // add a Position if necessary
    {
        newPositionAdded = true;
        position = new Position(positionIndex, 8, 0);
        staff->InsertPosition(voice, position);
    }

    position->InsertNote(new Note(stringNum, fretNumber));
}

void AddNote::undo()
{
    position->RemoveNote(stringNum);

    if (newPositionAdded)
    {
        staff->RemovePosition(voice, positionIndex);
    }
}
