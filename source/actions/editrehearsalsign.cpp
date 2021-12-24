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

#include "editrehearsalsign.h"

#include <score/barline.h>
#include <score/score.h>

AddRehearsalSign::AddRehearsalSign(const ScoreLocation &location,
                                   const std::string &description)
    : QUndoCommand(tr("Add Rehearsal Sign")),
      myLocation(location),
      myDescription(description)
{
}

void
AddRehearsalSign::redo()
{
    myLocation.getBarline()->setRehearsalSign(
        RehearsalSign("A", myDescription));
    ScoreUtils::adjustRehearsalSigns(myLocation.getScore());
}

void
AddRehearsalSign::undo()
{
    myLocation.getBarline()->clearRehearsalSign();
    ScoreUtils::adjustRehearsalSigns(myLocation.getScore());
}

EditRehearsalSign::EditRehearsalSign(const ScoreLocation &location,
                                     const std::string &new_description)
    : QUndoCommand(tr("Edit Rehearsal Sign")),
      myLocation(location),
      myNewDescription(new_description),
      myOrigSign(location.getBarline()->getRehearsalSign())
{
}

void
EditRehearsalSign::redo()
{
    RehearsalSign sign = myOrigSign;
    sign.setDescription(myNewDescription);
    myLocation.getBarline()->setRehearsalSign(sign);
}

void
EditRehearsalSign::undo()
{
    myLocation.getBarline()->setRehearsalSign(myOrigSign);
}

RemoveRehearsalSign::RemoveRehearsalSign(const ScoreLocation &location)
    : QUndoCommand(tr("Remove Rehearsal Sign")),
      myLocation(location),
      myRehearsalSign(location.getBarline()->getRehearsalSign())
{
}

void
RemoveRehearsalSign::redo()
{
    myLocation.getBarline()->clearRehearsalSign();
    ScoreUtils::adjustRehearsalSigns(myLocation.getScore());
}

void
RemoveRehearsalSign::undo()
{
    myLocation.getBarline()->setRehearsalSign(myRehearsalSign);
    ScoreUtils::adjustRehearsalSigns(myLocation.getScore());
}
