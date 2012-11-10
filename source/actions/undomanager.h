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
  
#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

#include <QUndoGroup>
#include <QUndoStack>

#include <boost/ptr_container/ptr_vector.hpp>

class QUndoCommand;

class UndoManager : public QUndoGroup
{
    Q_OBJECT
public:
    explicit UndoManager(QObject *parent = 0);

    void addNewUndoStack();
    void setActiveStackIndex(int index);
    void removeStack(int index);
    void push(QUndoCommand* cmd, int affectedSystem);
    void beginMacro(const QString& text);
    void endMacro();

    static const int AFFECTS_ALL_SYSTEMS = -1;

signals:
    void fullRedrawNeeded();
    void redrawNeeded(int);

private:
    void push(QUndoCommand* cmd);
    void onSystemChanged(int affectedSystem);

    boost::ptr_vector<QUndoStack> undoStacks;
};

class SignalOnRedo : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    virtual void redo();

signals:
    void triggered();
};

class SignalOnUndo: public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    virtual void undo();

signals:
    void triggered();
};

#endif // UNDOMANAGER_H
