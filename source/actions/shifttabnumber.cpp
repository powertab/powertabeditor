#include "shifttabnumber.h"

#include <powertabdocument/position.h>

ShiftTabNumber::ShiftTabNumber(Position* currentPos, Note* note, int direction, quint8 numStringsInStaff, const Tuning& tuning) :
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
}

void ShiftTabNumber::undo()
{
    int oppositeDirection = (direction == Position::SHIFT_UP) ? Position::SHIFT_DOWN : Position::SHIFT_UP;
    currentPos->ShiftTabNumber(note, oppositeDirection, numStringsInStaff, tuning);
}
