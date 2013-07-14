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
  
#ifndef PAINTERS_CLEFPAINTER_H
#define PAINTERS_CLEFPAINTER_H

#include <QFont>
#include <QGraphicsItem>
#include <QStaticText>
#include <score/staff.h>

class ClefPainter : public QGraphicsItem
{
public:
    ClefPainter(Staff::ClefType clefType, const QFont &musicFont);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *);
    virtual QRectF boundingRect() const { return myBounds; }

private:
    const Staff::ClefType myClefType;
    const QFont myMusicFont;
    QStaticText myDisplayText;
    QRectF myBounds;
};

#endif
