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
                               const QBrush &brush)
    : myText(text), myFont(font), myBrush(brush)
{
    QFontMetricsF fm(myFont);
    myAscent = fm.ascent();
    myBoundingRect = QRectF(0, 0, fm.width(myText), fm.height());
}

void SimpleTextItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    painter->setBrush(myBrush);
    painter->setFont(myFont);
    // Match the way that QSimpleTextItem aligns text.
    painter->drawText(0, myAscent, myText);
}