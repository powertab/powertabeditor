#include "positionshift.h"

#include <powertabdocument/system.h>

PositionShift::PositionShift(System* system, quint32 positionIndex, ShiftType type) :
    system(system),
    positionIndex(positionIndex),
    type(type)
{
    if (type == SHIFT_FORWARD)
    {
        setText(QObject::tr("Shift Forward"));
    }
    else
    {
        setText(QObject::tr("Shift Backward"));
    }
}

void PositionShift::redo()
{
    if (type == SHIFT_FORWARD)
    {
        system->ShiftForward(positionIndex);
    }
    else
    {
        system->ShiftBackward(positionIndex);
    }
}

void PositionShift::undo()
{
    if (type == SHIFT_FORWARD)
    {
        system->ShiftBackward(positionIndex);
    }
    else
    {
        system->ShiftForward(positionIndex);
    }
}
