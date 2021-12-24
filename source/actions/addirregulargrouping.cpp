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

#include "addirregulargrouping.h"

#include <score/voice.h>

AddIrregularGrouping::AddIrregularGrouping(const ScoreLocation &location,
                                           const IrregularGrouping &group)
    : myLocation(location), myGroup(group)
{
    if (group.getNotesPlayed() == 3 && group.getNotesPlayed() == 2)
        setText(tr("Add Triplet"));
    else
        setText(tr("Add Irregular Grouping"));
}

void AddIrregularGrouping::redo()
{
    myLocation.getVoice().insertIrregularGrouping(myGroup);
}

void AddIrregularGrouping::undo()
{
    myLocation.getVoice().removeIrregularGrouping(myGroup);
}
