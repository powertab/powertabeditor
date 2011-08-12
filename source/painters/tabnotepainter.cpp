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
  
#include "tabnotepainter.h"

#include <QPainter>
#include <QFontMetricsF>

#include <powertabdocument/note.h>

QFont TabNotePainter::tabFont = QFont("Liberation Sans");

TabNotePainter::TabNotePainter(Note* note) :
    note(note)
{
    tabFont.setPixelSize(10); // needed for cross-platform consistency in font size
    tabFont.setStyleStrategy(QFont::PreferAntialias);

    textColor = note->IsTied() ? Qt::lightGray : Qt::black;

    init();
}

void TabNotePainter::init()
{
    QString noteText = QString().fromStdString(note->GetText());

    displayText.setText(noteText);
    displayText.prepare(QTransform(), tabFont);

    QFontMetricsF fm(tabFont);
    bounds = QRectF(0, 0, fm.width(noteText), fm.height());
}

void TabNotePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(tabFont);
    painter->setPen(textColor);

    // offset height by 1 pixel for clarity
    painter->drawStaticText(0, 0, displayText);
}
