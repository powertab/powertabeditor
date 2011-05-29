#ifndef EDITREST_H
#define EDITREST_H

#include <QUndoCommand>

class Position;

class EditRest : public QUndoCommand
{
public:
    EditRest(Position* position, uint8_t duration);

    void redo();
    void undo();

private:
    Position* position;
    const uint8_t newDuration;
    const uint8_t originalDuration;
    const bool wasAlreadyRest;
};

#endif // EDITREST_H
