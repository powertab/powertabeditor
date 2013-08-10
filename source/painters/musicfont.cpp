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
  
#include "musicfont.h"

#include <QGraphicsSimpleTextItem>
#include <QFontDatabase>
#include <QString>

MusicFont::MusicFont()
{
    musicNotationFont = QFont("Emmentaler");
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}

/// Sets the text of the QGraphicsSimpleTextItem to the given music symbol
void MusicFont::setSymbol(QGraphicsSimpleTextItem* text, MusicSymbol identifier, int size)
{
    text->setText(QChar(identifier));
    musicNotationFont.setPixelSize(size);
    text->setFont(musicNotationFont);
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}

/// Returns a reference to the font
const QFont& MusicFont::getFont() const
{
    return musicNotationFont;
}
