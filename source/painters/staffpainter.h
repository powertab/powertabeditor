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

#include <memory>
#include <painters/layoutinfo.h>
#include <QGraphicsItem>
#include <score/scorelocation.h>

class ScoreClickEvent;
class Staff;

class StaffPainter : public QGraphicsItem
{
public:
    StaffPainter(const LayoutConstPtr &layout,
                 const ConstScoreLocation &location,
                 const ScoreClickEvent &click_event,
                 const QColor staffColor);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *) override;

    virtual QRectF boundingRect() const override
    {
        return myBounds;
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void drawStaffLines(QPainter *painter, int lineCount, double lineSpacing,
                        double startHeight);
    int getPositionFromX(double x) const;

    LayoutConstPtr myLayout;
    const ScoreClickEvent &myClickEvent;
    ConstScoreLocation myLocation;
    const QRectF myBounds;
    const QColor myStaffColor;
};

#endif
