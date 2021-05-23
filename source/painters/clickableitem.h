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

#ifndef PAINTERS_CLICKABLEITEM_H
#define PAINTERS_CLICKABLEITEM_H

#include "scoreclickevent.h"
#include <QGraphicsItem>
#include <score/scorelocation.h>

/// This class can either be a subclass of QGraphicsItem or QGraphicsItemGroup.
template <typename GraphicsItemT>
class ClickableItemT : public GraphicsItemT
{
public:
    ClickableItemT(const QString &tooltip, const ScoreClickEvent &click_event,
                   const ConstScoreLocation &location, ScoreItem item);

    virtual void mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                                const QVariant &value) override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

protected:
    virtual bool filterMousePosition(const QPointF &) const
    {
        return true;
    }

    const ScoreClickEvent &myClickEvent;
    const ConstScoreLocation myLocation;
    const ScoreItem myItem;
    int myPendingHoverLeaveEvents;
};

using ClickableGroup = ClickableItemT<QGraphicsItemGroup>;
using ClickableItem = ClickableItemT<QGraphicsItem>;

#endif

