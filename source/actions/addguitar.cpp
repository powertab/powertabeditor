#include "addguitar.h"

#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <widgets/mixer/mixer.h>

AddGuitar::AddGuitar(Score* score, Mixer* mixer) :
    score(score),
    mixer(mixer)
{
    newGuitar = std::make_shared<Guitar>();
    setText(QObject::tr("Add Guitar"));
}

void AddGuitar::redo()
{
    score->InsertGuitar(newGuitar);
    mixer->addInstrument(newGuitar);

    emit triggered();
}

void AddGuitar::undo()
{
    const size_t index = score->GetGuitarCount() - 1;

    score->RemoveGuitar(index);
    mixer->removeInstrument(index);

    emit triggered();
}
