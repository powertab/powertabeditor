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
  
#ifndef RESTPAINTER_H
#define RESTPAINTER_H

#include <QGraphicsItem>
#include <QStaticText>

class Position;

class RestPainter : public QGraphicsItem
{
public:
    RestPainter(const Position& position, QGraphicsItem* parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void init();

    const Position& position;

    QStaticText textToDraw; /// store the rest symbol that will be drawn
    const QFont musicFont;
    QRectF bounds;
    int restHeight;
};

#endif // RESTPAINTER_H
