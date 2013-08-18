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

#ifndef PAINTERS_NOTESTEM_H
#define PAINTERS_NOTESTEM_H

#include <score/position.h>

class NoteStem
{
public:
    enum StemType
    {
        StemUp,
        StemDown
    };

    NoteStem(const Position &pos, double x, double noteHeadWidth,
             const std::vector<double> &noteLocations);

    double getX() const;
    void setX(double x);
    double getTop() const;
    void setTop(double top);
    double getBottom() const;
    void setBottom(double bottom);
    double getStemEdge() const;
    double getStemHeight() const;

    double getNoteHeadRightEdge() const;
    double getNoteHeadWidth() const;

    double getDurationTime() const;
    Position::DurationType getDurationType() const;
    bool isStaccato() const;
    bool hasFermata() const;
    bool hasSforzando() const;
    bool hasMarcato() const;

    /// Returns true if the note should be part of a beam group.
    static bool isBeamable(const NoteStem &stem);
    /// Returns true if the note should have a stem drawn.
    static bool needsStem(const NoteStem &stem);

    StemType getStemType() const;
    void setStemType(StemType type);

private:
    const Position *myPosition;
    double myX;
    double myNoteHeadWidth;
    double myTop;
    double myBottom;
    StemType myStemType;
};

#endif
