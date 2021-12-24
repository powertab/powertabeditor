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

#ifndef ACTIONS_REMOVEPOSITION_H
#define ACTIONS_REMOVEPOSITION_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/irregulargrouping.h>
#include <score/position.h>
#include <score/scorelocation.h>

class RemovePosition : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemovePosition)

public:
    RemovePosition(const ScoreLocation &location,
                   const QString &text = tr("Remove Position"));

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const Position myOriginalPosition;
    std::vector<IrregularGrouping> myIrregularGroups;
};

#endif
