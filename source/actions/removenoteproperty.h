/*
  * Copyright (C) 2013 Cameron White
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
  
#ifndef ACTIONS_REMOVENOTEPROPERTY_H
#define ACTIONS_REMOVENOTEPROPERTY_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/note.h>
#include <score/scorelocation.h>

/// Removes a simple note property for each of the selected notes.
class RemoveNoteProperty : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveNoteProperty)

public:
    RemoveNoteProperty(const ScoreLocation &location,
                       Note::SimpleProperty property,
                       const QString &description);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const Note::SimpleProperty myProperty;
};

#endif
