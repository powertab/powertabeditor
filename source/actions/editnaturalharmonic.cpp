#include "editnaturalharmonic.h"

#include <powertabdocument/note.h>

EditNaturalHarmonic::EditNaturalHarmonic(Note* note, bool addHarmonic) :
    note(note),
    addHarmonic(addHarmonic)
{
    if (addHarmonic)
    {
        setText(QObject::tr("Set Natural Harmonic"));
    }
    else
    {
        setText(QObject::tr("Remove Natural Harmonic"));
    }
}

void EditNaturalHarmonic::redo()
{
    note->SetNaturalHarmonic(addHarmonic);
}

void EditNaturalHarmonic::undo()
{
    note->SetNaturalHarmonic(!addHarmonic);
}
