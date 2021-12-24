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

#ifndef ACTIONS_EDITCLEF_H
#define ACTIONS_EDITCLEF_H

#include <optional>
#include <QCoreApplication>
#include <QUndoCommand>
#include <score/scorelocation.h>
#include <score/system.h>

class EditStaff : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(EditStaff)

public:
    EditStaff(const ScoreLocation &location, Staff::ClefType clef, int strings);

    virtual void redo() override;
    virtual void undo() override;

private:
    static void addPlayerChangeAtStart(Score &score, int system_index);

    ScoreLocation myLocation;
    System myOriginalSystem;
    std::optional<System> myOriginalNextSystem;
    Staff::ClefType myClef;
    int myNumStrings;
};

#endif
