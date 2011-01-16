#ifndef ADDCHORDTEXT_H
#define ADDCHORDTEXT_H

#include <QUndoCommand>

class ChordText;
class System;

class AddChordText : public QUndoCommand
{
public:
    AddChordText(System* system, ChordText* chordText, quint32 index);
    ~AddChordText();
    virtual void undo();
    virtual void redo();

protected:
    ChordText* chordText;
    System* system;
    const quint32 index;
    bool chordTextInUse;
};

#endif // ADDCHORDTEXT_H
