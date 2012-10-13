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

#include "notestem.h"

#include <algorithm>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

NoteStem::NoteStem(const Position* position, double xPosition,
                   const std::vector<int>& noteLocations,
                   double noteHeadWidth, double noteHeadRightEdge) :
    position(position),
    xPosition(xPosition),
    noteHeadWidth(noteHeadWidth),
    noteHeadRightEdge(noteHeadRightEdge)
{
    stemTop = *std::min_element(noteLocations.begin(), noteLocations.end());
    stemBottom = *std::max_element(noteLocations.begin(), noteLocations.end());

    stemDirection = Staff::STD_NOTATION_LINE_SPACING * 2 < stemBottom ? StemUp : StemDown;
}

double NoteStem::stemSize() const
{
    double stemSize = Staff::STD_NOTATION_LINE_SPACING * 3.5;

    if (position->IsAcciaccatura())
    {
        stemSize *= 0.75;
    }

    return stemSize;
}

/// Returns the y-coordinate of the edge of the stem (where it meets the connecting beam)
double NoteStem::stemEdge() const
{
    return (stemDirection == StemUp) ? stemTop : stemBottom;
}

/// Functor to compare a NoteStem's stem direction
struct CompareStemDirection
{
    CompareStemDirection(NoteStem::StemDirection direction) : direction_(direction)
    {
    }

    bool operator()(const NoteStem& stem) const
    {
        return stem.stemDirection == direction_;
    }

    NoteStem::StemDirection direction_;
};

/// Finds the most common stem direction for a group of NoteStem's
NoteStem::StemDirection NoteStem::findDirectionForGroup(const std::vector<NoteStem> &stems)
{
    // Find how many stem directions of each type we have
    const size_t stemsUp = std::count_if(stems.begin(), stems.end(),
                                         CompareStemDirection(NoteStem::StemUp));

    const size_t stemsDown = std::count_if(stems.begin(), stems.end(),
                                           CompareStemDirection(NoteStem::StemDown));

    return (stemsDown >= stemsUp) ? NoteStem::StemDown : NoteStem::StemUp;
}

bool compareStemTopPositions(const NoteStem &stem1, const NoteStem &stem2)
{
    return stem1.stemTop < stem2.stemTop;
}

bool compareStemBottomPositions(const NoteStem &stem1, const NoteStem &stem2)
{
    return stem1.stemBottom < stem2.stemBottom;
}

/// Returns true if we can draw a flag for this note stem (must be eighth note or higher, or a grace note)
bool NoteStem::canDrawFlag() const
{
    return (position->GetDurationType() > 4) || position->IsAcciaccatura();
}
