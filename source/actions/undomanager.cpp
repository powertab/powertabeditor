/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "undomanager.h"

UndoManager::UndoManager(QObject *parent) :
    QUndoGroup(parent)
{
}

void UndoManager::addNewUndoStack()
{
    undoStacks.emplace_back(new QUndoStack);
    addStack(undoStacks.back().get());
}

void UndoManager::setActiveStackIndex(int index)
{
    if (index == -1) // When there are no open documents, the index is -1.
        return;

    setActiveStack(undoStacks.at(index).get());
}

void UndoManager::removeStack(int index)
{
    // Stack is automatically removed from the QUndoGroup when it is deleted.
    undoStacks.erase(undoStacks.begin() + index);
}

void UndoManager::push(QUndoCommand *cmd)
{
    activeStack()->push(cmd);
}

void UndoManager::push(QUndoCommand *cmd, int affectedSystem)
{
    beginMacro(cmd->actionText());

    auto onUndo = new SignalOnUndo();
    if (affectedSystem >= 0)
    {
        connect(onUndo, &SignalOnUndo::triggered, [=]() {
            onSystemChanged(affectedSystem);
        });
    }
    else
    {
        connect(onUndo, &SignalOnUndo::triggered, this,
                &UndoManager::fullRedrawNeeded);
    }

    push(onUndo);
    push(cmd);

    auto onRedo = new SignalOnRedo();
    if (affectedSystem >= 0)
    {
        connect(onRedo, &SignalOnRedo::triggered, [=]() {
            onSystemChanged(affectedSystem);
        });
    }
    else
    {
        connect(onRedo, &SignalOnRedo::triggered, this,
                &UndoManager::fullRedrawNeeded);
    }

    push(onRedo);
    endMacro();
}

void UndoManager::setClean()
{
    activeStack()->setClean();
}

void UndoManager::onSystemChanged(int affectedSystem)
{
    emit redrawNeeded(affectedSystem);
}

void UndoManager::beginMacro(const QString &text)
{
    activeStack()->beginMacro(text);
}

void UndoManager::endMacro()
{
    activeStack()->endMacro();
}

void SignalOnRedo::redo()
{
    emit triggered();
}

void SignalOnUndo::undo()
{
    emit triggered();
}
