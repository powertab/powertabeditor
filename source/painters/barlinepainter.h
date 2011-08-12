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
  
#ifndef BARLINEPAINTER_H
#define BARLINEPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

#include <memory>

class Barline;

class BarlinePainter : public QObject, public PainterBase
{
    Q_OBJECT

public:
    BarlinePainter(StaffData staffInfo, std::shared_ptr<const Barline> barLinePtr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void clicked(int);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void init();
    void drawVerticalLines(QPainter* painter, double x);

    StaffData staffInfo;
    std::shared_ptr<const Barline> barLine;
    double x;
    double width;

    static const double DOUBLE_BAR_WIDTH;
};

#endif // BARLINEPAINTER_H
