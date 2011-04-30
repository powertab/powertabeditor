#include "addchordtext.h"

#include <powertabdocument/system.h>

using std::shared_ptr;

AddChordText::AddChordText(shared_ptr<System> system, shared_ptr<ChordText> chordText, quint32 index) :
    chordText(chordText),
    system(system),
    index(index)
{
    setText(QObject::tr("Add Chord Text"));
}

void AddChordText::redo()
{
    system->InsertChordText(chordText, index);
}

void AddChordText::undo()
{
    system->RemoveChordText(index);
}
