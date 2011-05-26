#ifndef ADDTAPPEDHARMONIC_H
#define ADDTAPPEDHARMONIC_H

#include <QUndoCommand>
#include <cstdint>

class Note;

class AddTappedHarmonic : public QUndoCommand
{
public:
    AddTappedHarmonic(Note* note, uint8_t tappedFret);

    void redo();
    void undo();

private:
    Note* note;
    const uint8_t tappedFret;
};

#endif // ADDTAPPEDHARMONIC_H
