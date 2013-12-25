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
  
#include "removesystem.h"

#include <app/caret.h>
#include <score/score.h>

RemoveSystem::RemoveSystem(Score &score, int index, Caret &caret)
    : QUndoCommand(QObject::tr("Remove System")),
      myScore(score),
      myIndex(index),
      myCaret(caret),
      myOriginalSystem(score.getSystems()[index])
{
}

void RemoveSystem::redo()
{
    myScore.removeSystem(myIndex);
    // Move the caret to a valid system.
    myCaret.moveToSystem(std::min<int>(myIndex, myScore.getSystems().size()),
                         true);
}

void RemoveSystem::undo()
{
    myScore.insertSystem(myOriginalSystem, myIndex);
    myCaret.moveToSystem(myIndex, true);
}
