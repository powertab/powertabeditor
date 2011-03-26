#ifndef DELETEBARLINE_H
#define DELETEBARLINE_H

#include <QUndoCommand>
#include <memory>

class Barline;
class System;

class DeleteBarline : public QUndoCommand
{
public:
    DeleteBarline(std::shared_ptr<System> system, Barline* barline);
    ~DeleteBarline();
    void redo();
    void undo();

protected:
    std::shared_ptr<System> system;
    Barline* barline;
    Barline* barlineCopy;
};

#endif // DELETEBARLINE_H
