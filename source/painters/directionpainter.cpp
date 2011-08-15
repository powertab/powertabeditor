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
  
#include "directionpainter.h"

#include <QPainter>
#include <QFontMetricsF>
#include <powertabdocument/direction.h>
#include <painters/musicfont.h>

DirectionPainter::DirectionPainter(boost::shared_ptr<const Direction> direction, size_t symbolIndex) :
    direction(direction),
    symbolIndex(symbolIndex),
    usingMusicFont(true)
{
    init();
}

void DirectionPainter::init()
{
    QString text;
    
    uint8_t symbolType = 0, activeSymbol = 0, repeatNumber = 0;
    direction->GetSymbol(symbolIndex, symbolType, activeSymbol, repeatNumber);
    
    displayFont = MusicFont().getFont();
    displayFont.setPixelSize(20);
    displayFont.setStyleStrategy(QFont::PreferAntialias);
    usingMusicFont = true;
    
    // display music symbols for coda and segno directions, but display plain text otherwise
    
    switch(symbolType)
    {
    case Direction::coda:
        text = QChar(MusicFont::Coda);
        break;
    case Direction::doubleCoda:
        text = QString(2, MusicFont::Coda);
        break;
    case Direction::segno:
        text = QChar(MusicFont::Segno);
        break;
    case Direction::segnoSegno:
        text = QString(2, MusicFont::Segno);
        break;
    default:
        // switch to regular font
        displayFont = QFont("Liberation Sans");
        displayFont.setPixelSize(10);
        displayFont.setItalic(true);
        displayFont.setStyleStrategy(QFont::PreferAntialias);
        usingMusicFont = false;
        
        text = QString::fromStdString(direction->GetText(symbolIndex));
        break;
    }

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    QFontMetricsF fm(displayFont);
    bounds = QRectF(0, 0, fm.width(text), fm.height());
}

void DirectionPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(displayFont);
    
    if (usingMusicFont) // need to offset the text if we're using the music font
    {
        painter->drawStaticText(0, -(displayFont.pixelSize()), displayText);
    }
    else
    {
        painter->drawStaticText(0, 0, displayText);
    }
}
