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

#include <QGraphicsItem>
#include "staffdata.h"
#include <boost/shared_ptr.hpp>

class Position;
class Tuning;
class QPainter;
class Note;
class KeySignature;
class Staff;

class StdNotationPainter : public QGraphicsItem
{
public:
    StdNotationPainter(const StaffData& staffInfo,
                       boost::shared_ptr<const Staff> staff,
                       const Position* position, const Note* note,
                       const Tuning& tuning, const KeySignature& keySignature);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
    QRectF boundingRect() const;

    enum AccidentalType
    {
        NO_ACCIDENTAL,
        NATURAL,
        SHARP,
        DOUBLE_SHARP,
        FLAT,
        DOUBLE_FLAT
    };

    double getYLocation() const;
    const Position* getPositionObject() const;
    double noteHeadWidth() const;
    double noteHeadRightEdge() const;

    void refreshAccidental(bool forceAccidental);

    AccidentalType accidental;

private:
    void init();
    void setNoteHead();
    AccidentalType findAccidentalType(const QString& noteText) const;
    QString getAccidentalText() const;
    void addDots(QPainter* painter, double x, double y) const;

    QRectF bounds;

    StaffData staffInfo;
    boost::shared_ptr<const Staff> staff;
    const Position* position;
    const Note* note;
    const Tuning& tuning;
    const KeySignature& keySignature;

    static QFont musicFont;
    double yLocation;
    double xLocation;
    double rightEdge;
    QChar noteHead;
};

#endif // STDNOTATIONPAINTER_H
