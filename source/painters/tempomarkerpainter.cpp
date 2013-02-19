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

TempoMarkerPainter::TempoMarkerPainter(boost::shared_ptr<const TempoMarker> tempoMarker):
    tempoMarker(tempoMarker)
{
    displayFont.setPixelSize(10);
    displayFont.setBold(true);

    QFontMetricsF fm(displayFont);
    const qreal fontHeight = fm.height();

    QString text;
    text += QString::fromStdString(tempoMarker->GetDescription()) + " ";

    const QString imageSpacing(3, ' ');
    // Reserve space for the beat type image.
    beatTypeRect = QRectF(fm.width(text), -HEIGHT_OFFSET, fm.width(imageSpacing),
                          fontHeight + HEIGHT_OFFSET);
    text += imageSpacing;
    text += " = ";

    text += QString::number(tempoMarker->GetBeatsPerMinute());

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    bounds = QRectF(0, 0, fm.width(text), fontHeight);
}

void TempoMarkerPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHints(QPainter::SmoothPixmapTransform);
    painter->setFont(displayFont);
    painter->drawStaticText(0, HEIGHT_OFFSET, displayText);

    QPixmap image(getBeatTypeImage());
    painter->drawPixmap(beatTypeRect, image, image.rect());
}

QString TempoMarkerPainter::getBeatTypeImage() const
{
    QString file;

    switch (tempoMarker->GetBeatType())
    {
    case TempoMarker::half:
        file = ":images/half_note";
        break;
    case TempoMarker::halfDotted:
        file = ":images/half_note_dotted";
        break;
    case TempoMarker::quarter:
        file = ":images/quarter_note";
        break;
    case TempoMarker::quarterDotted:
        file = ":images/dotted_note";
        break;
    case TempoMarker::eighth:
        file = ":images/8th_note";
        break;
    case TempoMarker::eighthDotted:
        file = ":images/8th_note_dotted";
        break;
    case TempoMarker::sixteenth:
        file = ":images/16th_note";
        break;
    case TempoMarker::sixteenDotted:
        file = ":images/16th_note_dotted";
        break;
    case TempoMarker::thirtySecond:
        file = ":images/32nd_note";
        break;
    case TempoMarker::thirtySecondDotted:
        file = ":images/32nd_note_dotted";
        break;
    }

    return file;
}
