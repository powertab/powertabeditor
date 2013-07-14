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
  
#ifndef PAINTERS_STAFFPAINTER_H
#define PAINTERS_STAFFPAINTER_H

#include <painters/layoutinfo.h>
#include <QGraphicsItem>

class Staff;

class StaffPainter : public QGraphicsItem
{
public:
    StaffPainter(const LayoutConstPtr &layout);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *);
    virtual QRectF boundingRect() const { return myBounds; }

#if 0
signals:
    void selectionUpdated(int selectionStart, int selectionEnd);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
#endif
private:
    void drawStaffLines(QPainter *painter, int lineCount, double lineSpacing,
                        double startHeight);

#if 0
    inline int findClosestPosition(qreal click, qreal relativePos, qreal spacing);
#endif

    LayoutConstPtr myLayout;
    const QRectF myBounds;
};

#endif
