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
  
#ifndef STDNOTATIONPAINTER_H
#define STDNOTATIONPAINTER_H

#include "painterbase.h"
#include "staffdata.h"
#include <boost/shared_ptr.hpp>

class Position;
class Tuning;
class QPainter;
class Note;
class KeySignature;
class Staff;

class StdNotationPainter : public PainterBase
{
public:
    StdNotationPainter(const StaffData& staffInfo, boost::shared_ptr<const Staff> staff, const Position* position, const Note* note,
                       const Tuning& tuning, const KeySignature& keySignature);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    enum AccidentalType
    {
        NO_ACCIDENTAL,
        NATURAL,
        SHARP,
        DOUBLE_SHARP,
        FLAT,
        DOUBLE_FLAT
    };

    inline double getYLocation() const { return yLocation; }
    inline const Position* getPositionObject() const { return position; }

    static double getNoteHeadWidth();

    int accidental;

protected:
    void init();
    int findAccidentalType(const QString& noteText) const;
    QString getAccidentalText() const;
    void addDots(QPainter* painter, double x, double y) const;

    StaffData staffInfo;
    boost::shared_ptr<const Staff> staff;
    const Position* position;
    const Note* note;
    const Tuning& tuning;
    const KeySignature& keySignature;
    static QFont musicFont;
    double yLocation;
    double width;
};

#endif // STDNOTATIONPAINTER_H
