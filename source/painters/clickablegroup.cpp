/*
  * Copyright (C) 2014 Cameron White
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
  
#include "clickablegroup.h"

#include <QCursor>

ClickableGroup::ClickableGroup(const QString &tooltip, const Callback &callback)
    : myCallback(callback)
{
    setToolTip(tooltip);
    setAcceptHoverEvents(true);
}

void ClickableGroup::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    // No action is needed here, but we need to override this method in
    // order to get the mouse release event.
}

void ClickableGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    myCallback();
}

void ClickableGroup::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickableGroup::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    unsetCursor();
}
