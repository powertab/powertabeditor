/*
 * Copyright (C) 2020 Cameron White
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

#include "clickableitem.h"
#include "styles.h"

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <type_traits>

template <typename GraphicsItemT>
ClickableItemT<GraphicsItemT>::ClickableItemT(
    const QString &tooltip, const ScoreClickEvent &click_event,
    const ConstScoreLocation &location, ScoreItem item)
    : myClickEvent(click_event),
      myLocation(location),
      myItem(item),
      myPendingHoverLeaveEvents(0)
{
    this->setAcceptHoverEvents(true);
    this->setToolTip(tooltip);

    // Enable selection. The default implementations of mousePressEvent() and
    // mouseReleaseEvent() handle selection already.
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

template <typename GraphicsItemT>
void
ClickableItemT<GraphicsItemT>::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent *event)
{
    if (!filterMousePosition(event->pos()))
    {
        event->ignore();
        return;
    }

    myClickEvent.signal(myItem, myLocation, ScoreItemAction::DoubleClicked);
}

template <typename GraphicsItemT>
void
ClickableItemT<GraphicsItemT>::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (filterMousePosition(event->pos()))
        this->setCursor(Qt::PointingHandCursor);

    ++myPendingHoverLeaveEvents;
}

template <typename GraphicsItemT>
void
ClickableItemT<GraphicsItemT>::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (filterMousePosition(event->pos()))
        this->setCursor(Qt::PointingHandCursor);
    else
        this->unsetCursor();
}

template <typename GraphicsItemT>
void
ClickableItemT<GraphicsItemT>::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    // It seems possible to get multiple calls to hoverEnterEvent() with
    // QGraphicsItemGroup, so wait until the last hoverLeaveEvent() call.
    --myPendingHoverLeaveEvents;
    if (myPendingHoverLeaveEvents == 0)
        this->unsetCursor();
}

template <typename GraphicsItemT>
QVariant
ClickableItemT<GraphicsItemT>::itemChange(
    QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged && value.toBool())
        myClickEvent.signal(myItem, myLocation, ScoreItemAction::Selected);

    return QGraphicsItem::itemChange(change, value);
}

template <typename GraphicsItemT>
void
ClickableItemT<GraphicsItemT>::paint(QPainter *painter,
                                     const QStyleOptionGraphicsItem *option,
                                     [[maybe_unused]] QWidget *widget)
{
    // Override how the selection is drawn (default is to draw a black border).
    if (option->state & QStyle::State_Selected)
    {
        painter->fillRect(this->boundingRect(),
                          Styles::SelectionColor);
    }

    // Invoke the base class paint method if it's an actual QGraphicsItem
    // subclass.
    if constexpr (!std::is_same<GraphicsItemT, QGraphicsItem>::value &&
                  !std::is_same<GraphicsItemT, QGraphicsItemGroup>::value)
    {
        GraphicsItemT::paint(painter, option, widget);
    }
}

template class ClickableItemT<QGraphicsItem>;
template class ClickableItemT<QGraphicsItemGroup>;
template class ClickableItemT<QGraphicsPathItem>;
