#include "changebarlinetype.h"

#include "../powertabdocument/barline.h"

ChangeBarLineType::ChangeBarLineType(Barline* bar, quint8 barType, quint8 repeats)
{
    setText(QObject::tr("Change Barline Type"));
    barLine = bar;
    originalType = barLine->GetType();
    originalRepeatCount = barLine->GetRepeatCount();
    type = barType;
    repeatCount = repeats;
}

void ChangeBarLineType::undo()
{
    barLine->SetBarlineData(originalType, originalRepeatCount);
}

void ChangeBarLineType::redo()
{
    barLine->SetBarlineData(type, repeatCount);
}
