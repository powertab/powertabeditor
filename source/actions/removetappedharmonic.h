#ifndef REMOVETAPPEDHARMONIC_H
#define REMOVETAPPEDHARMONIC_H

#include <QUndoCommand>

class Note;

class RemoveTappedHarmonic : public QUndoCommand
{
public:
    RemoveTappedHarmonic(Note* note);
    void redo();
    void undo();

private:
    Note* note;
    uint8_t originalFret;
};

#endif // REMOVETAPPEDHARMONIC_H
