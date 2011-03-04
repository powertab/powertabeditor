#ifndef ADDTRILL_H
#define ADDTRILL_H

#include <QUndoCommand>

class Note;

class AddTrill : public QUndoCommand
{
public:
    AddTrill(Note* note, quint8 trillFret);
    void undo();
    void redo();

protected:
    Note* note;
    quint8 trillFret;
};

#endif // ADDTRILL_H
