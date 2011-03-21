#ifndef DELETEBARLINE_H
#define DELETEBARLINE_H

#include <QUndoCommand>

class Barline;
class System;

class DeleteBarline : public QUndoCommand
{
public:
    DeleteBarline(System* system, Barline* barline);
    ~DeleteBarline();
    void redo();
    void undo();

protected:
    System* system;
    Barline* barline;
    Barline* barlineCopy;
};

#endif // DELETEBARLINE_H
