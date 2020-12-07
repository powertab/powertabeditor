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

#include "styles.h"

#include <QCursor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

ClickableGroup::ClickableGroup(const QString &tooltip, const Callback &callback)
    : myCallback(callback)
{
    setToolTip(tooltip);
    setAcceptHoverEvents(true);

    // Enable selection. The default implementations of mousePressEvent() and
    // mouseReleaseEvent() handle selection already.
    setFlag(ItemIsSelectable, true);
}

void ClickableGroup::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
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

void
ClickableGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *)
{
    // Override how the selection is drawn (default is to draw a black border).
    if (option->state & QStyle::State_Selected)
        painter->fillRect(boundingRect(), Styles::SelectionColor);
}
