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
  
#include "chordtextpainter.h"

#include <QPainter>
#include <QFontMetricsF>

#include <powertabdocument/chordtext.h>

QFont ChordTextPainter::displayFont = QFont("Liberation Sans");

ChordTextPainter::ChordTextPainter(std::shared_ptr<const ChordText> chordText) :
    chordText(chordText)
{
    displayFont.setPixelSize(10); // needed for cross-platform consistency in font size
    displayFont.setStyleStrategy(QFont::PreferAntialias);

    init();
}

void ChordTextPainter::init()
{
    const QString text = QString::fromStdString(chordText->GetText());

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    QFontMetricsF fm(displayFont);
    bounds = QRectF(0, 0, fm.width(text), fm.height());
}

void ChordTextPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(displayFont);

    painter->drawStaticText(0, 0, displayText);
}
