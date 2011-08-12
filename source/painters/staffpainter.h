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
  
#ifndef STAFFPAINTER_H
#define STAFFPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

#include <memory>
#include <QPen>

class Staff;
class System;

class StaffPainter : public QObject, public PainterBase
{
    Q_OBJECT

public:
    StaffPainter(std::shared_ptr<const System> system, std::shared_ptr<const Staff> staff,
                 const StaffData& staffInfo);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void selectionUpdated(int selectionStart, int selectionEnd);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int drawStaffLines(int lineCount, int lineSpacing, int startHeight);
    inline int findClosestPosition(qreal click, qreal relativePos, qreal spacing);

    std::shared_ptr<const System> system;
    std::shared_ptr<const Staff> staff;
    StaffData staffInfo;
    QPen pen;

    int selectionStart;
    int selectionEnd;

    void init();
};

#endif // STAFFPAINTER_H
