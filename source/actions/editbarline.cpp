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
  
#include "editbarline.h"

EditBarline::EditBarline(const ScoreLocation &location, Barline::BarType type,
                         int repeats)
    : QUndoCommand(tr("Edit Barline Type")),
      myLocation(location),
      myBarType(type),
      myOriginalBarType(location.getBarline()->getBarType()),
      myRepeats(repeats),
      myOriginalRepeats(location.getBarline()->getRepeatCount())
{
}

void EditBarline::redo()
{
    myLocation.getBarline()->setBarType(myBarType);
    myLocation.getBarline()->setRepeatCount(myRepeats);
}

void EditBarline::undo()
{
    myLocation.getBarline()->setBarType(myOriginalBarType);
    myLocation.getBarline()->setRepeatCount(myOriginalRepeats);
}
