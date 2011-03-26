#include "removesystem.h"

#include <powertabdocument/score.h>

RemoveSystem::RemoveSystem(Score* score, quint32 index) :
    score(score),
    index(index)
{
    systemCopy = score->GetSystem(index);
    setText(QObject::tr("Remove System"));
}

void RemoveSystem::redo()
{
    score->RemoveSystem(index);
    emit triggered();
}

void RemoveSystem::undo()
{
    score->InsertSystem(systemCopy, index);
    emit triggered();
}
