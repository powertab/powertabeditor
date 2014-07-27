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
  
#include "clefpainter.h"

#include <app/pubsub/staffpubsub.h>
#include <painters/musicfont.h>

ClefPainter::ClefPainter(Staff::ClefType clefType, const QFont &musicFont,
                         int system, int staff,
                         std::shared_ptr<StaffPubSub> pubsub)
    : SimpleTextItem(clefType == Staff::TrebleClef
                         ? QChar(MusicFont::TrebleClef)
                         : QChar(MusicFont::BassClef),
                     musicFont),
      mySystemIndex(system),
      myStaffIndex(staff),
      myPubSub(pubsub)
{
    setAcceptHoverEvents(true);
}

void ClefPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    // No action is needed here, but we need to override this method in order
    // to get the mouse release event.
}

void ClefPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    myPubSub->publish(mySystemIndex, myStaffIndex);
}

void ClefPainter::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    setCursor(Qt::PointingHandCursor);
}

void ClefPainter::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    unsetCursor();
}