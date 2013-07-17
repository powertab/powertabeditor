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

#include <QGraphicsItem>
#include <boost/shared_ptr.hpp>
#include <painters/layoutinfo.h>
#include <score/scorelocation.h>

class Barline;
class ScoreLocationPubSub;

class BarlinePainter : public QGraphicsItem
{
public:
    BarlinePainter(const LayoutConstPtr& layout, const Barline &barline,
                   const ScoreLocation& location,
                   boost::shared_ptr<ScoreLocationPubSub> pubsub);

    QRectF boundingRect() const { return myBounds; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

    bool isInStdNotationStaff(double y);
    void drawVerticalLines(QPainter *painter, double myX);

    LayoutConstPtr myLayout;
    const Barline &myBarline;
    QRectF myBounds;
    ScoreLocation myLocation;
    boost::shared_ptr<ScoreLocationPubSub> myPubSub;
    double myX;
    double myWidth;

    static const double DOUBLE_BAR_WIDTH;
};

#endif // BARLINEPAINTER_H
