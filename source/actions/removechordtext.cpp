#include "removechordtext.h"

#include <powertabdocument/system.h>
#include <powertabdocument/chordtext.h>

RemoveChordText::RemoveChordText(System* system, quint32 index) :
    system(system), index(index)
{
    setText(QObject::tr("Remove Chord Text"));
    chordText = system->GetChordText(index);
    chordTextInUse = true;
}

RemoveChordText::~RemoveChordText()
{
    if (!chordTextInUse)
    {
        delete chordText;
    }
}

void RemoveChordText::redo()
{
    system->RemoveChordText(index);
    chordTextInUse = false;
}

void RemoveChordText::undo()
{
    system->InsertChordText(chordText, index);
    chordTextInUse = true;
}
