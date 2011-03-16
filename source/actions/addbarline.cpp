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
    // TODO
}

void AddBarline::undo()
{
    // TODO
}
