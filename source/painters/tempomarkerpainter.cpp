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

#include <QPainter>
#include <score/tempomarker.h>

TempoMarkerPainter::TempoMarkerPainter(const TempoMarker &tempo)
    : myTempoMarker(tempo),
      myDisplayFont("Liberation Sans")
{
    Q_ASSERT(myTempoMarker.getMarkerType() != TempoMarker::NotShown);

    myDisplayFont.setPixelSize(10);
    if (myTempoMarker.getMarkerType() == TempoMarker::AlterationOfPace)
        myDisplayFont.setItalic(true);
    else
        myDisplayFont.setBold(true);

    QFontMetricsF fm(myDisplayFont);
    const double fontHeight = fm.height();

    QString text;
    if (myTempoMarker.getMarkerType() == TempoMarker::AlterationOfPace)
    {
        if (myTempoMarker.getAlterationOfPace() == TempoMarker::Accelerando)
            text = "accel.";
        else
            text = "rit.";
    }
    else
    {
        text += QString::fromStdString(myTempoMarker.getDescription()) + " ";

        const QString imageSpacing(3, ' ');
        // Reserve space for the beat type image.
        myBeatTypeRect =
            QRectF(fm.width(text), -HEIGHT_OFFSET, fm.width(imageSpacing),
                   fontHeight + HEIGHT_OFFSET);
        text += imageSpacing;
        text += " = ";
        text += QString::number(myTempoMarker.getBeatsPerMinute());
    }

    myDisplayText.setText(text);
    myDisplayText.prepare(QTransform(), myDisplayFont);

    myBounds = QRectF(0, 0, fm.width(text), fontHeight);
}

void TempoMarkerPainter::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHints(QPainter::SmoothPixmapTransform);
    painter->setFont(myDisplayFont);
    painter->drawStaticText(0, HEIGHT_OFFSET, myDisplayText);

    if (myTempoMarker.getMarkerType() == TempoMarker::StandardMarker)
    {
        QPixmap image(getBeatTypeImage());
        painter->drawPixmap(myBeatTypeRect, image, image.rect());
    }
}

QString TempoMarkerPainter::getBeatTypeImage() const
{
    QString file;

    switch (myTempoMarker.getBeatType())
    {
    case TempoMarker::Half:
        file = ":images/half_note";
        break;
    case TempoMarker::HalfDotted:
        file = ":images/half_note_dotted";
        break;
    case TempoMarker::Quarter:
        file = ":images/quarter_note";
        break;
    case TempoMarker::QuarterDotted:
        file = ":images/dotted_note";
        break;
    case TempoMarker::Eighth:
        file = ":images/8th_note";
        break;
    case TempoMarker::EighthDotted:
        file = ":images/8th_note_dotted";
        break;
    case TempoMarker::Sixteenth:
        file = ":images/16th_note";
        break;
    case TempoMarker::SixteenthDotted:
        file = ":images/16th_note_dotted";
        break;
    case TempoMarker::ThirtySecond:
        file = ":images/32nd_note";
        break;
    case TempoMarker::ThirtySecondDotted:
        file = ":images/32nd_note_dotted";
        break;
    }

    return file;
}
