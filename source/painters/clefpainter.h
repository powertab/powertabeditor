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
  
#ifndef PAINTERS_CLEFPAINTER_H
#define PAINTERS_CLEFPAINTER_H

#include <memory>
#include <painters/simpletextitem.h>
#include <score/staff.h>

class StaffPubSub;

class ClefPainter : public SimpleTextItem
{
public:
    ClefPainter(Staff::ClefType clefType, const QFont &musicFont, int system,
                int staff, std::shared_ptr<StaffPubSub> pubsub);

private:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    const int mySystemIndex;
    const int myStaffIndex;
    std::shared_ptr<StaffPubSub> myPubSub;
};

#endif
