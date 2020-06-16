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
  
#include "simpletextitem.h"

#include <QPainter>

SimpleTextItem::SimpleTextItem(const QString &text, const QFont &font,
                               TextAlignment alignment, const QPen &pen,
                               const QBrush &background)
    : myText(text),
      myFont(font),
      myPen(pen),
      myBackground(background),
      myAlignment(alignment)
{
    QFontMetricsF fm(myFont);
    myAscent = fm.ascent();
    switch (myAlignment)
    {
        case TextAlignment::Top:
            myBoundingRect = QRectF(0, 0, fm.width(myText), fm.height());
            break;
        case TextAlignment::Baseline:
            myBoundingRect = fm.boundingRect(text);
            break;
    }
}

void SimpleTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                           QWidget *)
{
    // Draw the background rectangle. Avoid to cover other elements
    // by drawing only 1/3 of the rectangle, vertically centered.
    painter->fillRect(
                myBoundingRect.x(),
                myBoundingRect.y() + myBoundingRect.height() / 3,
                myBoundingRect.width(),
                myBoundingRect.height() / 3,
                myBackground);

    painter->setPen(myPen);
    painter->setFont(myFont);

    switch (myAlignment)
    {
        case TextAlignment::Top:
            painter->drawText(0, myAscent, myText);
            break;
        case TextAlignment::Baseline:
            painter->drawText(0, 0, myText);
            break;
    }
}
