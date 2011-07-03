#include "edittuning.h"

#include <powertabdocument/guitar.h>

EditTuning::EditTuning(std::shared_ptr<Guitar> guitar, const Tuning& newTuning) :
    guitar(guitar),
    newTuning(newTuning),
    oldTuning(guitar->GetTuning())
{
    setText(QObject::tr("Edit Tuning"));
}

void EditTuning::redo()
{
    guitar->SetTuning(newTuning);
}

void EditTuning::undo()
{
    guitar->SetTuning(oldTuning);
}
