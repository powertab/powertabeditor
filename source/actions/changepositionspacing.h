#ifndef CHANGEPOSITIONSPACING_H
#define CHANGEPOSITIONSPACING_H

#include <QUndoCommand>

class System;

class ChangePositionSpacing : public QUndoCommand
{
public:
    ChangePositionSpacing(System* system, quint8 spacing);
    void undo();
    void redo();

protected:
    System* system;
    const quint8 newSpacing;
    const quint8 originalSpacing;
};

#endif // CHANGEPOSITIONSPACING_H
