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

#include <powertabdocument/note.h>

QFont TabNotePainter::tabFont = QFont("Liberation Sans");
QFontMetricsF TabNotePainter::fontMetrics(TabNotePainter::tabFont);

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
    const QString noteText = QString::fromStdString(note->GetText());

    displayText.setText(noteText);
    displayText.prepare(QTransform(), tabFont);

    bounds = QRectF(0, 0, fontMetrics.width(noteText), fontMetrics.height());
}

void TabNotePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(tabFont);
    painter->setPen(textColor);

    painter->drawStaticText(0, 0, displayText);
}

QRectF TabNotePainter::boundingRect() const
{
    return bounds;
}
