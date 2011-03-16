#include "addbarline.h"

#include <powertabdocument/barline.h>
#include <powertabdocument/system.h>

AddBarline::AddBarline(System* system, quint32 position, quint8 type, quint8 repeats) :
    system(system),
    position(position),
    type(type),
    repeats(repeats)
{
    setText(QObject::tr("Insert Barline"));
}

void AddBarline::redo()
{
    Barline* newBar = new Barline(position, type, repeats);
    system->InsertBarline(newBar);
}

void AddBarline::undo()
{
    system->RemoveBarline(position);
}
