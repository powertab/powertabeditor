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
  
#ifndef PAINTERS_DIRECTIONPAINTER_H
#define PAINTERS_DIRECTIONPAINTER_H

#include <QFont>
#include <QGraphicsItem>
#include <QStaticText>

class DirectionSymbol;

class DirectionPainter : public QGraphicsItem
{
public:
    DirectionPainter(const DirectionSymbol &symbol);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *);
    virtual QRectF boundingRect() const { return myBounds; }

private:
    const DirectionSymbol &mySymbol;
    QFont myDisplayFont;
    QStaticText myDisplayText;
    bool myUsingMusicFont;
    QRectF myBounds;
};

#endif
