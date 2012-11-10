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

#include <boost/bind.hpp>
#include <sigfwd/sigfwd.hpp>

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

/// Pushes the QUndoCommand onto the active stack.
void UndoManager::push(QUndoCommand* cmd)
{
    activeStack()->push(cmd);
}

/// Pushes an undo command onto the active stack.
/// @param affectedSystem Index of the system that is modified by this action.
/// Use -1 for actions that affect all systems.
void UndoManager::push(QUndoCommand* cmd, int affectedSystem)
{
    beginMacro(cmd->actionText());

    SignalOnUndo* onUndo = new SignalOnUndo();
    if (affectedSystem >= 0)
    {
        sigfwd::connect(onUndo, SIGNAL(triggered()),
                        boost::bind(&UndoManager::onSystemChanged, this, affectedSystem));
    }
    else
    {
        connect(onUndo, SIGNAL(triggered()), this, SIGNAL(fullRedrawNeeded()));
    }
    push(onUndo);

    push(cmd);

    SignalOnRedo* onRedo = new SignalOnRedo();
    if (affectedSystem >= 0)
    {
        sigfwd::connect(onRedo, SIGNAL(triggered()),
                        boost::bind(&UndoManager::onSystemChanged, this, affectedSystem));
    }
    else
    {
        connect(onRedo, SIGNAL(triggered()), this, SIGNAL(fullRedrawNeeded()));
    }
    push(onRedo);

    endMacro();
}

void UndoManager::onSystemChanged(int affectedSystem)
{
    emit redrawNeeded(affectedSystem);
}

void UndoManager::beginMacro(const QString& text)
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
