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
  
#ifndef ACTIONS_INSERTNOTES_H
#define ACTIONS_INSERTNOTES_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/irregulargrouping.h>
#include <score/position.h>
#include <score/scorelocation.h>
#include <vector>

class InsertNotes : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(InsertNotes)

public:
    InsertNotes(const ScoreLocation &location,
                const std::vector<Position> &positions,
                const std::vector<IrregularGrouping> &groups);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    std::vector<Position> myNewPositions;
    std::vector<IrregularGrouping> myNewGroups;
    int myShiftAmount;
};

#endif
