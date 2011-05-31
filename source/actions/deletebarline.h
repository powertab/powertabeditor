#ifndef DELETEBARLINE_H
#define DELETEBARLINE_H

#include <QUndoCommand>
#include <memory>

class Barline;
class System;

class DeleteBarline : public QUndoCommand
{
public:
    DeleteBarline(std::shared_ptr<System> system, std::shared_ptr<Barline> barline);
    void redo();
    void undo();

protected:
    std::shared_ptr<System> system;
    std::shared_ptr<Barline> barline;
};

#endif // DELETEBARLINE_H
