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
  
#include "restpainter.h"

#include <QPainter>

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include "musicfont.h"

const QFont RestPainter::musicFont = MusicFont().getFont();

RestPainter::RestPainter(const Position& position, QGraphicsItem* parent) :
    QGraphicsItem(parent),
    position(position)
{
     // position is approximately in middle of staff by default (negative offset necessary due to font spacing)
    restHeight = -2 * Staff::STD_NOTATION_LINE_SPACING - 2;

    QChar restSymbol;
    switch(position.GetDurationType()) // find the correct symbol to display, and adjust the height if necessary
    {
    case 1:
        restSymbol = MusicFont::WholeRest;
        restHeight -= Staff::STD_NOTATION_LINE_SPACING - 1;
        break;
    case 2:
        restSymbol = MusicFont::HalfRest;
        break;
    case 4:
        restSymbol = MusicFont::QuarterRest;
        break;
    case 8:
        restSymbol = MusicFont::EighthRest;
        break;
    case 16:
        restSymbol = MusicFont::SixteenthRest;
        break;
    case 32:
        restSymbol = MusicFont::ThirtySecondRest;
        break;
    case 64:
        restSymbol = MusicFont::SixtyFourthRest;
        restHeight -= 3; // adjust for extra height of rest
        break;
    }

    textToDraw.setText(restSymbol);
    textToDraw.prepare(QTransform(), musicFont);

    bounds = QRectF(0, 0, 2 * textToDraw.textWidth(), musicFont.pixelSize());
}

QRectF RestPainter::boundingRect() const
{
    return bounds;
}

void RestPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    // draw rest symbol
    painter->setFont(musicFont);
    painter->drawStaticText(0, restHeight, textToDraw);

    // draw dots if necessary
    const QChar dot = MusicFont::getSymbol(MusicFont::Dot);
    const double x = musicFont.pixelSize() / 2;
    const double y = 1.6 * Staff::STD_NOTATION_LINE_SPACING; // just below second line of staff

    if (position.IsDotted())
    {
        painter->drawText(x, y, dot);
    }
    else if (position.IsDoubleDotted())
    {
        painter->drawText(x, y, dot);
        painter->drawText(x + 4, y, dot);
    }
}
