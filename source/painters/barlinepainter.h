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
  
#ifndef PAINTERS_BARLINEPAINTER_H
#define PAINTERS_BARLINEPAINTER_H

#include "clickableitem.h"

#include <QGraphicsItem>
#include <memory>
#include <painters/layoutinfo.h>
#include <score/scorelocation.h>

class Barline;
class ScoreClickEvent;

class BarlinePainter : public ClickableItem
{
public:
    BarlinePainter(const LayoutConstPtr &layout, const Barline &barline,
                   const ConstScoreLocation &location,
                   const ScoreClickEvent &click_event,
                   const QColor &barlineColor);

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual QRectF boundingRect() const override
    {
        return myBounds;
    }

protected:
    bool filterMousePosition(const QPointF &pos) const override;

private:
    void drawVerticalLines(QPainter *painter, double myX);

    LayoutConstPtr myLayout;
    const Barline &myBarline;
    QRectF myBounds;
    double myX;
    double myWidth;
    const QColor &myBarlineColor;

    static const double DOUBLE_BAR_WIDTH;
};

#endif // BARLINEPAINTER_H
