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

#include <painters/musicfont.h>
#include <QPainter>
#include <QFontMetricsF>
#include <score/direction.h>

static QString theDirectionText[] = {
    "Coda", "Double Coda", "Segno", "Segno Segno",
    "Fine", "D.C.", "D.S.", "D.S.S.", "To Coda",
    "To Dbl. Coda", "D.C. al Coda", "D.C. al Dbl. Coda",
    "D.S. al Coda", "D.S. al Dbl. Coda", "D.S.S. al Coda",
    "D.S.S. al Dbl. Coda", "D.C. al Fine", "D.S. al Fine",
    "D.S.S. al Fine"
};

DirectionPainter::DirectionPainter(const DirectionSymbol &symbol)
    : mySymbol(symbol),
      myUsingMusicFont(true)
{
    myDisplayFont = MusicFont().getFont();
    myDisplayFont.setPixelSize(20);
    myDisplayFont.setStyleStrategy(QFont::PreferAntialias);
    
    // Display music symbols for coda and segno directions, but display
    // plain text otherwise.
    QString text;

    switch (mySymbol.getSymbolType())
    {
    case DirectionSymbol::Coda:
        text = QChar(MusicFont::Coda);
        break;
    case DirectionSymbol::DoubleCoda:
        text = QString(2, MusicFont::Coda);
        break;
    case DirectionSymbol::Segno:
        text = QChar(MusicFont::Segno);
        break;
    case DirectionSymbol::SegnoSegno:
        text = QString(2, MusicFont::Segno);
        break;
    default:
        // Switch to regular font.
        myDisplayFont = QFont("Liberation Sans");
        myDisplayFont.setPixelSize(10);
        myDisplayFont.setItalic(true);
        myDisplayFont.setStyleStrategy(QFont::PreferAntialias);
        myUsingMusicFont = false;

        text = theDirectionText[mySymbol.getSymbolType()];
        break;
    }

    myDisplayText = text;

    QFontMetricsF fm(myDisplayFont);
    myBounds = QRectF(0, 0, fm.width(text), fm.height());
}

void DirectionPainter::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setFont(myDisplayFont);
    
    // Need to offset the text if we're using the music font.
    if (myUsingMusicFont)
        painter->drawText(0, myDisplayFont.pixelSize() / 2, myDisplayText);
    else
        painter->drawText(0, myDisplayFont.pixelSize() + 4, myDisplayText);
}
