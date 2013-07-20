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
  
#include "tremolobarpainter.h"

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <QPainter>
#include <QFontMetricsF>

TremoloBarPainter::TremoloBarPainter(const Position* position, double width) :
    position(position),
    width(width),
    textFont("Liberation Sans")
{
    textFont.setPixelSize(9);

    bounds = QRectF(0, 0, width, Staff::TAB_SYMBOL_HEIGHT);
}

void TremoloBarPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    Q_ASSERT(position->HasTremoloBar());

    uint8_t type = 0, duration = 0, pitch = 0;
    position->GetTremoloBar(type, duration, pitch);

    double displayWidth = width;
    if (type == Position::dip || type == Position::invertedDip)
    {
        displayWidth /= 2.0; // don't want to stretch a dip into the next position
    }

    const QString displayText = QString::fromStdString(position->GetTremoloBarText());

    const double topHeight = Staff::TAB_SYMBOL_HEIGHT / 2.0;
    const double bottomHeight = Staff::TAB_SYMBOL_HEIGHT;
    const double middleX = displayWidth / 2.0;

    switch(type)
    {
    case Position::dip:
        painter->drawLine(0, topHeight, middleX, bottomHeight);
        painter->drawLine(middleX, bottomHeight, displayWidth, topHeight);
        break;

    case Position::invertedDip:
        painter->drawLine(0, bottomHeight, middleX, topHeight);
        painter->drawLine(middleX, topHeight, displayWidth, bottomHeight);
        break;

    case Position::diveAndHold:
    case Position::diveAndRelease:
        painter->drawLine(0, topHeight, displayWidth, bottomHeight);
        break;

    case Position::returnAndHold:
    case Position::returnAndRelease:
        painter->drawLine(0, bottomHeight, displayWidth, topHeight);
        break;
    }

    if (type != Position::release)
    {
        painter->setFont(textFont);
        painter->drawText(centerItem(0, displayWidth, QFontMetricsF(textFont).width(displayText)),
                          topHeight - 2, displayText);
    }
}
