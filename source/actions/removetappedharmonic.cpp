#include "removetappedharmonic.h"

#include <powertabdocument/note.h>

RemoveTappedHarmonic::RemoveTappedHarmonic(Note* note) :
    note(note),
    originalFret(0)
{
    Q_ASSERT(note->HasTappedHarmonic());

    note->GetTappedHarmonic(originalFret);
    setText(QObject::tr("Remove Tapped Harmonic"));
}

void RemoveTappedHarmonic::redo()
{
    note->ClearTappedHarmonic();
}

void RemoveTappedHarmonic::undo()
{
    note->SetTappedHarmonic(originalFret);
}
