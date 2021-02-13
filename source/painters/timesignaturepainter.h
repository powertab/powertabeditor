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
  
#ifndef PAINTERS_TIMESIGNATUREPAINTER_H
#define PAINTERS_TIMESIGNATUREPAINTER_H

#include "clickableitem.h"

#include <memory>
#include <painters/layoutinfo.h>
#include <QGraphicsItem>
#include <score/scorelocation.h>

class ScoreClickEvent;
class TimeSignature;

class TimeSignaturePainter : public ClickableItem
{
public:
    TimeSignaturePainter(const LayoutConstPtr &layout,
                         const TimeSignature &time,
                         const ConstScoreLocation &location,
                         const ScoreClickEvent &click_event);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *) override;

    virtual QRectF boundingRect() const override
    {
        return myBounds;
    }

private:
    void drawNumber(QPainter* painter, const double y, const int number) const;

    LayoutConstPtr myLayout;
    const TimeSignature &myTimeSignature;
    const QRectF myBounds;
};

#endif // TIMESIGNATUREPAINTER_H
