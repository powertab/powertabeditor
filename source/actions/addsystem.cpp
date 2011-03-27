#include "addsystem.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

AddSystem::AddSystem(Score* score, quint32 index) :
    score(score),
    index(index)
{
    setText(QObject::tr("Add System"));
}

void AddSystem::redo()
{
    std::shared_ptr<System> newSystem(new System);

    // adjust the location of the system (should be below the previous system)
    if (index != 0)
    {
        const Rect prevRect = score->GetSystem(index - 1)->GetRect();

        Rect currentRect = newSystem->GetRect();
        currentRect.SetTop(prevRect.GetBottom() + Score::SYSTEM_SPACING);
        newSystem->SetRect(currentRect);
    }

    score->InsertSystem(newSystem, index);

    emit triggered();
}

void AddSystem::undo()
{
    score->RemoveSystem(index);

    emit triggered();
}
