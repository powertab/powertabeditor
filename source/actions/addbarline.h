#ifndef ADDBARLINE_H
#define ADDBARLINE_H

#include <QUndoCommand>

class System;

class AddBarline : public QUndoCommand
{
public:
    AddBarline(System* system, quint32 position, quint8 type, quint8 repeats);
    void redo();
    void undo();

protected:
    System* system;
    const quint32 position;
    const quint8 type;
    const quint8 repeats;
};

#endif // ADDBARLINE_H
