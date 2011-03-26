#include "changepositionspacing.h"

#include <powertabdocument/system.h>

using std::shared_ptr;

ChangePositionSpacing::ChangePositionSpacing(shared_ptr<System> system, quint8 spacing) :
    system(system),
    newSpacing(spacing),
    originalSpacing(system->GetPositionSpacing())
{
    if (newSpacing < originalSpacing)
    {
        setText(QObject::tr("Decrease Position Spacing"));
    }
    else
    {
        setText(QObject::tr("Increase Position Spacing"));
    }
}

void ChangePositionSpacing::redo()
{
    system->SetPositionSpacing(newSpacing);
}

void ChangePositionSpacing::undo()
{
    system->SetPositionSpacing(originalSpacing);
}
