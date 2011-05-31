#include "deletebarline.h"

#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>

DeleteBarline::DeleteBarline(std::shared_ptr<System> system, std::shared_ptr<Barline> barline) :
    system(system),
    barline(barline)
{
    setText(QObject::tr("Delete Barline"));
}

void DeleteBarline::redo()
{
    system->RemoveBarline(barline->GetPosition());
}

void DeleteBarline::undo()
{
    system->InsertBarline(barline);
}
