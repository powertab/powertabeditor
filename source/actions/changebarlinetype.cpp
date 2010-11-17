#include "changebarlinetype.h"

#include "../powertabdocument/barline.h"

ChangeBarLineType::ChangeBarLineType(Barline* bar, uint8_t barType, uint8_t repeats)
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
