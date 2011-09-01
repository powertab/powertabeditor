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

#ifndef NOTESTEM_H
#define NOTESTEM_H

#include <vector>

class Position;

class NoteStem
{
public:
    enum StemDirection
    {
        StemUp,
        StemDown
    };

    NoteStem(const Position* position, int xPosition, const std::vector<int>& noteLocations);

    const Position* position;
    int xPosition;
    int stemTop;
    int stemBottom;
    StemDirection stemDirection;

    float stemSize() const;
    int stemEdge() const;
    bool canDrawFlag() const;

    static NoteStem::StemDirection findDirectionForGroup(const std::vector<NoteStem>& stems);
};

bool compareStemTopPositions(const NoteStem& stem1, const NoteStem& stem2);
bool compareStemBottomPositions(const NoteStem& stem1, const NoteStem& stem2);

#endif // NOTESTEM_H
