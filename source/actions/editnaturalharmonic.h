#ifndef EDITNATURALHARMONIC_H
#define EDITNATURALHARMONIC_H

#include <QUndoCommand>

class Note;

class EditNaturalHarmonic : public QUndoCommand
{
public:
    EditNaturalHarmonic(Note* note, bool addHarmonic);
    virtual void undo();
    virtual void redo();

protected:
    Note* note;
    bool addHarmonic; // whether we are adding or removing the harmonic
};

#endif // EDITNATURALHARMONIC_H
