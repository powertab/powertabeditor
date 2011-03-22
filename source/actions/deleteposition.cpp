#include "deleteposition.h"

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>

DeletePosition::DeletePosition(Staff* staff, Position* position, quint32 voice) :
    staff(staff),
    position(position),
    voice(voice)
{
    positionCopy.reset(position->CloneObject());
    setText(QObject::tr("Clear Position"));
}

void DeletePosition::redo()
{
    staff->RemovePosition(voice, positionCopy->GetPosition());
}

void DeletePosition::undo()
{
    staff->InsertPosition(voice, positionCopy->CloneObject());
}
