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

#ifndef ACTIONS_EDITREHEARSALSIGN_H
#define ACTIONS_EDITREHEARSALSIGN_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/rehearsalsign.h>
#include <score/scorelocation.h>

/// Adds a new rehearsal sign.
class AddRehearsalSign : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddRehearsalSign)

public:
    AddRehearsalSign(const ScoreLocation &location,
                     const std::string &description);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const std::string myDescription;
};

/// Edits an existing rehearsal sign. This doesn't need to re-assign letters to
/// subsequent signs.
class EditRehearsalSign : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(EditRehearsalSign)

public:
    EditRehearsalSign(const ScoreLocation &location,
                      const std::string &new_description);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const std::string myNewDescription;
    RehearsalSign myOrigSign;
};

/// Removes a rehearsal sign.
class RemoveRehearsalSign : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveRehearsalSign)

public:
    RemoveRehearsalSign(const ScoreLocation &location);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const RehearsalSign myRehearsalSign;
};

#endif
