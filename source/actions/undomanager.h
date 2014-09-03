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
  
#ifndef ACTIONS_UNDOMANAGER_H
#define ACTIONS_UNDOMANAGER_H

#include <memory>
#include <QUndoGroup>
#include <QUndoStack>
#include <vector>

class QUndoCommand;

class UndoManager : public QUndoGroup
{
    Q_OBJECT

public:
    explicit UndoManager(QObject *parent = nullptr);

    void addNewUndoStack();
    void setActiveStackIndex(int index);
    void removeStack(int index);

    /// Pushes an undo command onto the active stack.
    /// @param affectedSystem Index of the system that is modified by this action.
    /// Use -1 for actions that affect all systems.
    void push(QUndoCommand *cmd, int affectedSystem);

    void setClean();

    void beginMacro(const QString &text);
    void endMacro();

    static const int AFFECTS_ALL_SYSTEMS = -1;

signals:
    void fullRedrawNeeded();
    void redrawNeeded(int);

private:
    /// Pushes the QUndoCommand onto the active stack.
    void push(QUndoCommand *cmd);

    void onSystemChanged(int affectedSystem);

    std::vector<std::unique_ptr<QUndoStack>> undoStacks;
};

class SignalOnRedo : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    virtual void redo() override;

signals:
    void triggered();
};

class SignalOnUndo: public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    virtual void undo() override;

signals:
    void triggered();
};

#endif
