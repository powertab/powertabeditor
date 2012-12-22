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

#include "removeirregulargrouping.h"

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>

RemoveIrregularGrouping::RemoveIrregularGrouping(boost::shared_ptr<Staff> staff,
                                                 Position* position,
                                                 uint32_t voice) :
    staff(staff),
    position(position),
    voice(voice),
    irregularGroupBounds(findIrregularGroup())
{
    Q_ASSERT(position->HasIrregularGroupingTiming());
    position->GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

    setText(QObject::tr("Remove Irregular Grouping"));
}

void RemoveIrregularGrouping::redo()
{
    for (size_t i = irregularGroupBounds.first;
         i <= irregularGroupBounds.second; i++)
    {
        Position* position = staff->GetPosition(voice, i);
        position->ClearIrregularGroupingTiming();
        position->SetIrregularGroupingEnd(false);
        position->SetIrregularGroupingMiddle(false);
        position->SetIrregularGroupingStart(false);
    }
}

void RemoveIrregularGrouping::undo()
{
    for (size_t i = irregularGroupBounds.first;
         i <= irregularGroupBounds.second; i++)
    {
        Position* position = staff->GetPosition(voice, i);
        position->SetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

        if (i == irregularGroupBounds.first)
        {
            position->SetIrregularGroupingStart();
        }
        else if (i == irregularGroupBounds.second)
        {
            position->SetIrregularGroupingEnd();
        }
        else
        {
            position->SetIrregularGroupingMiddle();
        }
    }
}

/// Given a single Position object that is in an irregular grouping, find
/// the index of the start/end note in that grouping
std::pair<size_t, size_t> RemoveIrregularGrouping::findIrregularGroup() const
{
    size_t left = 0;
    size_t right = staff->GetPositionCount(voice);

    if (position->IsIrregularGroupingStart())
    {
        left = staff->GetIndexOfPosition(voice, position);
        right = findGroupEnd(left);
    }
    else if (position->IsIrregularGroupingEnd())
    {
        right = staff->GetIndexOfPosition(voice, position);
        left = findGroupStart(right);
    }
    else
    {
        const size_t middle = staff->GetIndexOfPosition(voice, position);
        left = findGroupStart(middle);
        right = findGroupEnd(middle);
    }

    return std::make_pair(left, right);
}

/// Search for the end of an irregular group, starting from the given index
size_t RemoveIrregularGrouping::findGroupEnd(size_t startIndex) const
{
    for (size_t i = startIndex; i < staff->GetPositionCount(voice); i++)
    {
        if (staff->GetPosition(voice, i)->IsIrregularGroupingEnd())
        {
            return i;
        }
    }

    Q_ASSERT(false); // should have an end to the group
    return startIndex;
}

/// Search for the beginning of an irregular group, starting from the given index
size_t RemoveIrregularGrouping::findGroupStart(size_t startIndex) const
{
    for (int i = startIndex; i >= 0; i--)
    {
        if (staff->GetPosition(voice, i)->IsIrregularGroupingStart())
        {
            return i;
        }
    }

    Q_ASSERT(false); // should have a starting note for the group
    return startIndex;
}
