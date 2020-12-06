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
  
#include "staffpainter.h"

#include <app/pubsub/clickpubsub.h>
#include <cmath>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

StaffPainter::StaffPainter(const LayoutConstPtr &layout,
                           const ConstScoreLocation &location,
                           const std::shared_ptr<ClickPubSub> &pubsub,
                           const QColor staffColor)
    : myLayout(layout),
      myPubSub(pubsub),
      myLocation(location),
      myBounds(0, 0, LayoutInfo::STAFF_WIDTH, layout->getStaffHeight()),
      myStaffColor(staffColor)
{
    // Only use the left mouse button for making selections.
    setAcceptedMouseButtons(Qt::LeftButton);
}

void StaffPainter::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    const double x = event->pos().x();
    const double y = event->pos().y();

    // Find the position relative to the top of the staff, in terms of the tab
    // line spacing. Then, round it to find the string index.
    const int string = std::floor(((y - myLayout->getTopTabLine()) /
                                   myLayout->getTabLineSpacing()) + 0.5);

    if (string >= 0 && string < myLayout->getStringCount())
    {
        const int position = myLayout->getPositionFromX(x);
        myLocation.setPositionIndex(position);
        myLocation.setSelectionStart(position);
        myLocation.setString(string);

        myPubSub->publish(ClickType::Selection, myLocation);
    }
}

void StaffPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    const double x = event->pos().x();
    myLocation.setPositionIndex(myLayout->getPositionFromX(x));
    myPubSub->publish(ClickType::Selection, myLocation);
}

void StaffPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                         QWidget *)
{
    painter->setPen(QPen(QBrush(myStaffColor), 0.75));

    // Draw standard notation staff.
    drawStaffLines(painter, LayoutInfo::NUM_STD_NOTATION_LINES,
                   myLayout->STD_NOTATION_LINE_SPACING,
                   myLayout->getTopStdNotationLine());

    // Draw tab staff.
    drawStaffLines(painter, myLayout->getStringCount(),
                   myLayout->getTabLineSpacing(), myLayout->getTopTabLine());
}

void StaffPainter::drawStaffLines(QPainter *painter, int lineCount,
                                  double lineSpacing, double startHeight)
{
    for (int i = 0; i < lineCount; i++)
    {
        const double height = i * lineSpacing + startHeight;
        painter->drawLine(QPointF(0, height),
                          QPointF(LayoutInfo::STAFF_WIDTH, height));
    }
}
