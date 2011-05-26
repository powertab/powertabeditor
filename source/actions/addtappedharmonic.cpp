#include "addtappedharmonic.h"

#include <powertabdocument/note.h>

AddTappedHarmonic::AddTappedHarmonic(Note* note, uint8_t tappedFret) :
    note(note),
    tappedFret(tappedFret)
{
    setText(QObject::tr("Add Tapped Harmonic"));
}

void AddTappedHarmonic::redo()
{
    note->SetTappedHarmonic(tappedFret);
}

void AddTappedHarmonic::undo()
{
    note->ClearTappedHarmonic();
}
