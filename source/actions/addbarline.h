#ifndef ADDBARLINE_H
#define ADDBARLINE_H

#include <QUndoCommand>
#include <memory>

class System;
class Barline;

class AddBarline : public QUndoCommand
{
public:
    AddBarline(std::shared_ptr<System> system, quint32 position, quint8 type, quint8 repeats);
    void redo();
    void undo();

protected:
    std::shared_ptr<System> system;
    const quint32 position;
    const quint8 type;
    const quint8 repeats;
    std::shared_ptr<Barline> newBar;
};

#endif // ADDBARLINE_H
