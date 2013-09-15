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

#include <QUndoCommand>
#include <score/position.h>
#include <score/scorelocation.h>

class RemovePosition : public QUndoCommand
{
public:
    RemovePosition(const ScoreLocation &location);

    virtual void redo();
    virtual void undo();

private:
    ScoreLocation myLocation;
    const Position myOriginalPosition;
};

#endif
