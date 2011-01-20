#ifndef UPDATENOTEDURATION_H
#define UPDATENOTEDURATION_H

#include <QUndoCommand>

class Position;

class UpdateNoteDuration : public QUndoCommand
{
public:
    UpdateNoteDuration(Position* position, quint8 duration);
    virtual void undo();
    virtual void redo();

protected:
    Position* position;
    quint8 newDuration;
    quint8 originalDuration;
};

#endif // UPDATENOTEDURATION_H
