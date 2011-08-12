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
#include <functional>
#include "staffdata.h"

class Position;
class StaffData;
class QGraphicsItem;

class NoteStem
{
public:
    enum StemDirection
    {
        StemUp,
        StemDown
    };

    NoteStem(const StaffData& staffInfo, const Position* position,
             double x, const std::vector<double>& noteLocations);

    const Position* position() const;

    double x() const;
    void setX(double x);

    double top() const;
    void setTop(double top);

    double bottom() const;
    void setBottom(double bottom);

    StemDirection direction() const;
    void setDirection(StemDirection direction);

    double stemSize() const;

    QGraphicsItem* createNoteFlag() const;
    QGraphicsItem* createStaccato() const;
    QGraphicsItem* createFermata() const;
    QGraphicsItem* createAccent() const;

    double stemEdge() const;

    bool canDrawFlag() const;

private:
    const Position* position_;
    double x_;
    double top_;
    double bottom_;
    double stemSize_;
    StemDirection direction_;
    StaffData staffInfo_;
};

NoteStem::StemDirection findDirectionForGroup(const std::vector<NoteStem>& stems);

/// Compares either the top or bottom position of a stem, depending on the function
/// that is passed to the constructor
struct CompareStemPositions
{
    typedef std::function<double (const NoteStem&)> PositionGetter;

    CompareStemPositions(PositionGetter posFn) : posFn(posFn) {}

    bool operator()(const NoteStem& stem1, const NoteStem& stem2) const
    {
        return posFn(stem1) < posFn(stem2);
    }

    PositionGetter posFn;
};

#endif // NOTESTEM_H
