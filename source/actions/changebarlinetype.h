#ifndef CHANGEBARLINETYPE_H
#define CHANGEBARLINETYPE_H

#include <QUndoCommand>

class Barline;

// Modifys barline properties

class ChangeBarLineType : public QUndoCommand
{
public:
    ChangeBarLineType(Barline* bar, uint8_t barType, uint8_t repeats);
    virtual void undo();
    virtual void redo();

private:
    Barline* barLine;
    uint8_t type;
    uint8_t repeatCount;
    uint8_t originalType;
    uint8_t originalRepeatCount;
};

#endif // CHANGEBARLINETYPE_H
