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
#include "staff.h"
#include "position.h"

namespace PowerTabDocument {

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

/// Sets the stem directions for all stems in a group.
/// This depends on how many stems point upwards/downwards.
NoteStem::StemDirection NoteStem::setStemDirection(std::vector<NoteStem> &stems)
{
    NoteStem::StemDirection dir = findDirectionForGroup(stems);

    // Assign the new stem direction to each stem
    for (size_t i = 0; i < stems.size(); i++)
    {
        stems[i].stemDirection = dir;
    }

    return dir;
}

namespace
{
bool compareStemTopPositions(const NoteStem &stem1, const NoteStem &stem2)
{
    return stem1.stemTop < stem2.stemTop;
}

bool compareStemBottomPositions(const NoteStem &stem1, const NoteStem &stem2)
{
    return stem1.stemBottom < stem2.stemBottom;
}
}

NoteStem NoteStem::findHighestStem(const std::vector<NoteStem>& stems)
{
    return *std::min_element(stems.begin(), stems.end(), &compareStemTopPositions);
}

NoteStem NoteStem::findLowestStem(const std::vector<NoteStem>& stems)
{
    return *std::max_element(stems.begin(), stems.end(), &compareStemBottomPositions);
}

bool NoteStem::needsStem(const Position *pos)
{
    return pos->GetDurationType() != 1 && !pos->IsRest() &&
            !pos->HasMultibarRest() && pos->GetNoteCount() > 0;
}

/// Returns true if we can draw a flag for this note stem (must be eighth note or higher, or a grace note)
bool NoteStem::canDrawFlag() const
{
    return (position->GetDurationType() > 4) || position->IsAcciaccatura();
}

}
