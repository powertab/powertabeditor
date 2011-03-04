#ifndef REMOVETRILL_H
#define REMOVETRILL_H

#include <QUndoCommand>

class Note;

class RemoveTrill : public QUndoCommand
{
public:
    RemoveTrill(Note* note);
    void undo();
    void redo();

protected:
    Note* note;
    uint8_t originalTrill;

};

#endif // REMOVETRILL_H
