#ifndef CHANGEBARLINETYPE_H
#define CHANGEBARLINETYPE_H

#include <QUndoCommand>
#include <memory>

class Barline;

// Modifys barline properties

class ChangeBarLineType : public QUndoCommand
{
public:
    ChangeBarLineType(std::shared_ptr<Barline> bar, quint8 barType, quint8 repeats);
    virtual void undo();
    virtual void redo();

private:
    std::shared_ptr<Barline> barLine;
    quint8 type;
    quint8 repeatCount;
    quint8 originalType;
    quint8 originalRepeatCount;
};

#endif // CHANGEBARLINETYPE_H
