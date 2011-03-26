#include "addchordtext.h"

#include <powertabdocument/system.h>
#include <powertabdocument/chordtext.h>

using std::shared_ptr;

AddChordText::AddChordText(shared_ptr<System> system, ChordText* chordText, quint32 index) :
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
