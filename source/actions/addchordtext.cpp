#include "addchordtext.h"

#include <powertabdocument/system.h>
#include <powertabdocument/chordtext.h>

AddChordText::AddChordText(System* system, ChordText* chordText, quint32 index, QObject *parent) :
    chordText(chordText),
    system(system),
    index(index)
{
    setText(QObject::tr("Add Chord Text"));
    chordTextInUse = false;
}

AddChordText::~AddChordText()
{
    if (!chordTextInUse)
    {
        delete chordText;
    }
}

void AddChordText::redo()
{
    system->InsertChordText(chordText, index);
    chordTextInUse = true;
}

void AddChordText::undo()
{
    system->RemoveChordText(index);
    chordTextInUse = false;
}
