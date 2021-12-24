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
  
#include "editnoteduration.h"

EditNoteDuration::EditNoteDuration(const ScoreLocation &location,
                                   Position::DurationType duration,
                                   bool forRests)
    : QUndoCommand(forRests ? tr("Edit Rest Duration") :
                              tr("Edit Note Duration")),
      myLocation(location),
      myNewDuration(duration)
{
    for (const Position *pos : myLocation.getSelectedPositions())
        myOriginalDurations.push_back(pos->getDurationType());
}

void EditNoteDuration::redo()
{
    for (Position *pos : myLocation.getSelectedPositions())
        pos->setDurationType(myNewDuration);
}

void EditNoteDuration::undo()
{
    std::vector<Position *> selectedPositions = myLocation.getSelectedPositions();

    for (size_t i = 0; i < myOriginalDurations.size(); ++i)
        selectedPositions[i]->setDurationType(myOriginalDurations[i]);
}
