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
  
#include "tempomarkerpainter.h"

#include <powertabdocument/tempomarker.h>

#include <QPainter>

QFont TempoMarkerPainter::displayFont = QFont("Liberation Sans");

TempoMarkerPainter::TempoMarkerPainter(std::shared_ptr<const TempoMarker> tempoMarker):
    tempoMarker(tempoMarker)
{
    displayFont.setPixelSize(10);

    init();
}

void TempoMarkerPainter::init()
{
    QString text;
    text += QString::fromStdString(tempoMarker->GetDescription());
    text += " ";
    text += QString().setNum(tempoMarker->GetBeatsPerMinute());

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    QFontMetricsF fm(displayFont);
    bounds = QRectF(0, 0, fm.width(text), fm.height());
}

void TempoMarkerPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->setFont(displayFont);

    painter->drawStaticText(0, 0, displayText);
}
