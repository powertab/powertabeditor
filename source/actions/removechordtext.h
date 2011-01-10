#ifndef REMOVECHORDTEXT_H
#define REMOVECHORDTEXT_H

#include <QUndoCommand>

class ChordText;
class System;

// Removes chord text

class RemoveChordText : public QUndoCommand
{
public:
    RemoveChordText(System* system, quint32 index);
    ~RemoveChordText();
    virtual void undo();
    virtual void redo();

protected:
    ChordText* chordText;
    System* system;
    const quint32 index;
    bool chordTextInUse;
};

#endif // REMOVECHORDTEXT_H
