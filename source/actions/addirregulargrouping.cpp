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

#include <powertabdocument/position.h>

AddIrregularGrouping::AddIrregularGrouping(const std::vector<Position*>& positions,
                                           uint8_t notesPlayed, uint8_t notesPlayedOver) :
    positions(positions),
    notesPlayed(notesPlayed),
    notesPlayedOver(notesPlayedOver)
{
    if (notesPlayed == 3 && notesPlayedOver == 2)
    {
        setText(QObject::tr("Add Triplet"));
    }
    else
    {
        setText(QObject::tr("Add Irregular Grouping"));
    }
}

/// Set all notes to have the irregular grouping, and flag each note as
/// the beginning/middle/end of the group
void AddIrregularGrouping::redo()
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        Position* pos = positions[i];
        pos->SetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

        if (i == 0)
        {
            pos->SetIrregularGroupingStart();
        }
        else if (i == positions.size() - 1)
        {
            pos->SetIrregularGroupingEnd();
        }
        else
        {
            pos->SetIrregularGroupingMiddle();
        }
    }
}

/// Clear all properties related to irregular groupings
void AddIrregularGrouping::undo()
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        Position* pos = positions[i];

        pos->ClearIrregularGroupingTiming();
        pos->SetIrregularGroupingStart(false);
        pos->SetIrregularGroupingMiddle(false);
        pos->SetIrregularGroupingEnd(false);
    }
}
