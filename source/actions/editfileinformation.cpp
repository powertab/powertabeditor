/*
  * Copyright (C) 2012 Cameron White
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

#include "editfileinformation.h"

#include <score/score.h>

EditFileInformation::EditFileInformation(const ScoreLocation &location,
                                         const ScoreInfo &scoreInfo)
    : QUndoCommand(tr("Edit File Information")),
      myLocation(location),
      myScoreInfo(scoreInfo),
      myOriginalScoreInfo(location.getScore().getScoreInfo())
{
}

void EditFileInformation::redo()
{
    myLocation.getScore().setScoreInfo(myScoreInfo);
}

void EditFileInformation::undo()
{
    myLocation.getScore().setScoreInfo(myOriginalScoreInfo);
}
