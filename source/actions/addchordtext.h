#ifndef ADDCHORDTEXT_H
#define ADDCHORDTEXT_H

#include <QUndoCommand>
#include <memory>

class ChordText;
class System;

class AddChordText : public QUndoCommand
{
public:
    AddChordText(std::shared_ptr<System> system, std::shared_ptr<ChordText> chordText, quint32 index);
    virtual void undo();
    virtual void redo();

protected:
    std::shared_ptr<ChordText> chordText;
    std::shared_ptr<System> system;
    const quint32 index;
};

#endif // ADDCHORDTEXT_H
