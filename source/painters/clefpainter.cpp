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
  
#include "clefpainter.h"

#include <painters/musicfont.h>
#include <QPainter>

ClefPainter::ClefPainter(Staff::ClefType clefType, const QFont &musicFont)
    : myClefType(clefType),
      myMusicFont(musicFont)
{
    if (myClefType == Staff::TrebleClef)
        myDisplayText.setText(MusicFont::getSymbol(MusicFont::TrebleClef));
    else
        myDisplayText.setText(MusicFont::getSymbol(MusicFont::BassClef));

    myDisplayText.prepare(QTransform(), myMusicFont);
    QFontMetricsF fm(myMusicFont);
    myBounds = QRectF(0, -5, fm.width(MusicFont::TrebleClef), fm.height());
}

void ClefPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*,
                        QWidget*)
{
    painter->setFont(myMusicFont);

    if (myClefType == Staff::TrebleClef)
        painter->drawStaticText(0, -6, myDisplayText);
    else
        painter->drawStaticText(0, -21, myDisplayText);
}
