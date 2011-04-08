#include "undomanager.h"

UndoManager::UndoManager(QObject *parent) :
    QUndoGroup(parent)
{
}

void UndoManager::addNewUndoStack()
{
    undoStacks.push_back(new QUndoStack);
    addStack(&undoStacks.back());
}

void UndoManager::setActiveStackIndex(int index)
{
    if (index == -1) // when there are no open documents, the index is -1
    {
         return;
    }

    setActiveStack(&undoStacks.at(index));
}

void UndoManager::removeStack(int index)
{
    // stack is automatically removed from the QUndoGroup when it is deleted
    undoStacks.erase(undoStacks.begin() + index);
}

// Pushes the QUndoCommand onto the active stack
void UndoManager::push(QUndoCommand* cmd)
{
    activeStack()->push(cmd);
}
