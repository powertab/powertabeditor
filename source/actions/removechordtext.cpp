#include "removechordtext.h"

#include <powertabdocument/system.h>

using std::shared_ptr;

RemoveChordText::RemoveChordText(shared_ptr<System> system, quint32 index) :
    system(system),
    index(index)
{
    setText(QObject::tr("Remove Chord Text"));

    chordText = system->GetChordText(index);
}

void RemoveChordText::redo()
{
    system->RemoveChordText(index);
}

void RemoveChordText::undo()
{
    system->InsertChordText(chordText, index);
}
