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

#ifndef ACTIONS_EDITDYNAMIC_H
#define ACTIONS_EDITDYNAMIC_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/dynamic.h>
#include <score/scorelocation.h>

class EditDynamic : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(EditDynamic)

public:
    EditDynamic(const ScoreLocation &location,
                const Dynamic &originalDynamic,
                const Dynamic &newDynamic);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const Dynamic myOriginalDynamic;
    const Dynamic myNewDynamic;
};

#endif
