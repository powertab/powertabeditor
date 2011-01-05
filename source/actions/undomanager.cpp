#include "undomanager.h"

UndoManager::UndoManager(QObject *parent) :
    QUndoGroup(parent)
{
}

UndoManager::~UndoManager()
{
    // the QUndoGroup is not responsible for managing the memory
    // of the QUndoStacks that it contains
    qDeleteAll(undoStacks);
    undoStacks.clear();
}

void UndoManager::addNewUndoStack()
{
    QUndoStack* stack = new QUndoStack;

    undoStacks.append(stack);
    addStack(stack);
}

void UndoManager::setActiveStackIndex(int index)
{
    if (index == -1) // when there are no open documents, the index is -1
    {
         return;
    }

    QUndoStack* currentStack = undoStacks.at(index);

    setActiveStack(currentStack);
}

void UndoManager::removeStack(int index)
{
    QUndoStack* stack = undoStacks.at(index);
    delete stack; // stack is automatically removed from the group when it is deleted
    undoStacks.removeAt(index);
}

// Pushes the QUndoCommand onto the active stack
void UndoManager::push(QUndoCommand* cmd)
{
    activeStack()->push(cmd);
}
