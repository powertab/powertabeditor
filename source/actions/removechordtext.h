#ifndef REMOVECHORDTEXT_H
#define REMOVECHORDTEXT_H

#include <QUndoCommand>
#include <memory>

class ChordText;
class System;

// Removes chord text

class RemoveChordText : public QUndoCommand
{
public:
    RemoveChordText(std::shared_ptr<System> system, quint32 index);
    virtual void undo();
    virtual void redo();

protected:
    std::shared_ptr<ChordText> chordText;
    std::shared_ptr<System> system;
    const quint32 index;
};

#endif // REMOVECHORDTEXT_H
