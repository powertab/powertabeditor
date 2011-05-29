#include "editrest.h"

#include <powertabdocument/position.h>

EditRest::EditRest(Position* position, uint8_t duration) :
    position(position),
    newDuration(duration),
    originalDuration(position->GetDurationType()),
    wasAlreadyRest(position->IsRest())
{
    if (!wasAlreadyRest)
    {
        setText(QObject::tr("Add Rest"));
    }
    else if (originalDuration != newDuration)
    {
        setText(QObject::tr("Edit Rest Duration"));
    }
    else
    {
        setText(QObject::tr("Remove Rest"));
    }
}

void EditRest::redo()
{
    if (wasAlreadyRest && (newDuration == originalDuration))
    {
        position->SetRest(false);
    }
    else
    {
        position->SetRest(true);
        position->SetDurationType(newDuration);
    }
}

void EditRest::undo()
{
    position->SetRest(wasAlreadyRest);
    position->SetDurationType(originalDuration);
}
