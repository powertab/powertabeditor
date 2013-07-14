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

#include <boost/lexical_cast.hpp>
#include <QFontMetricsF>
#include <QPainter>
#include <score/note.h>

TabNotePainter::TabNotePainter(const Note &note)
    : myNote(note),
      myTextColor(note.hasProperty(Note::Tied) ? Qt::lightGray : Qt::black),
      myTabFont("Liberation Sans")
{
    myTabFont.setPixelSize(10);
    myTabFont.setStyleStrategy(QFont::PreferAntialias);

    const QString noteText = QString::fromStdString(
                boost::lexical_cast<std::string>(myNote));

    myDisplayText.setText(noteText);
    myDisplayText.prepare(QTransform(), myTabFont);

    QFontMetricsF fm(myTabFont);
    myBounds = QRectF(0, 0, fm.width(noteText), fm.height());
}

void TabNotePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(myTabFont);
    painter->setPen(myTextColor);

    painter->drawStaticText(0, 0, myDisplayText);
}
