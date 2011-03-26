#ifndef CHANGEPOSITIONSPACING_H
#define CHANGEPOSITIONSPACING_H

#include <QUndoCommand>
#include <memory>

class System;

class ChangePositionSpacing : public QUndoCommand
{
public:
    ChangePositionSpacing(std::shared_ptr<System> system, quint8 spacing);
    void undo();
    void redo();

protected:
    std::shared_ptr<System> system;
    const quint8 newSpacing;
    const quint8 originalSpacing;
};

#endif // CHANGEPOSITIONSPACING_H
