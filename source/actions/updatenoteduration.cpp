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
  
#include "updatenoteduration.h"

#include <powertabdocument/position.h>

UpdateNoteDuration::UpdateNoteDuration(const std::vector<Position*>& positions,
                                       uint8_t duration) :
    positions(positions),
    newDuration(duration)
{
    setText(QObject::tr("Update Note Duration"));

    // store original durations of each position
    for (size_t i = 0; i < positions.size(); i++)
    {
        originalDurations.push_back(positions[i]->GetDurationType());
    }

    Q_ASSERT(Position::IsValidDurationType(newDuration));
}

/// Set all positions to the new note duration
void UpdateNoteDuration::redo()
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        positions[i]->SetDurationType(newDuration);
    }
}

/// Restore the original duration for each position
void UpdateNoteDuration::undo()
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        positions[i]->SetDurationType(originalDurations[i]);
    }
}
