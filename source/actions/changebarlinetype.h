#ifndef CHANGEBARLINETYPE_H
#define CHANGEBARLINETYPE_H

#include <QUndoCommand>

class Barline;

// Modifys barline properties

class ChangeBarLineType : public QUndoCommand
{
public:
    ChangeBarLineType(Barline* bar, quint8 barType, quint8 repeats);
    virtual void undo();
    virtual void redo();

private:
    Barline* barLine;
    quint8 type;
    quint8 repeatCount;
    quint8 originalType;
    quint8 originalRepeatCount;
};

#endif // CHANGEBARLINETYPE_H
