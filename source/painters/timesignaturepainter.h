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
  
#ifndef TIMESIGNATUREPAINTER_H
#define TIMESIGNATUREPAINTER_H

#include "painterbase.h"
#include "staffdata.h"
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <powertabdocument/systemlocation.h>

class StaffData;
class TimeSignature;
class SystemLocationPubSub;

class TimeSignaturePainter : public PainterBase
{
public:
    TimeSignaturePainter(const StaffData& staffInformation,
                         const TimeSignature& signature,
                         const SystemLocation& location,
                         boost::shared_ptr<SystemLocationPubSub> pubsub);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void init();
    void drawNumber(QPainter* painter, const double y, const quint8 number) const;

    StaffData staffInfo;
    const TimeSignature& timeSignature;
    const SystemLocation location;
    boost::shared_ptr<SystemLocationPubSub> pubsub;
};

#endif // TIMESIGNATUREPAINTER_H
