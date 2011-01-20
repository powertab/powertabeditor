#include "updatenoteduration.h"

#include <powertabdocument/position.h>

UpdateNoteDuration::UpdateNoteDuration(Position* position, quint8 duration) :
    position(position),
    newDuration(duration)
{
    originalDuration = position->GetDurationType();

    Q_ASSERT(position != NULL);
    Q_ASSERT(position->IsValidDurationType(newDuration));
}

void UpdateNoteDuration::redo()
{
    position->SetDurationType(newDuration);
}

void UpdateNoteDuration::undo()
{
    position->SetDurationType(originalDuration);
}
