#include "tempomarkerpainter.h"

#include <powertabdocument/tempomarker.h>

#include <QPainter>

QFont TempoMarkerPainter::displayFont = QFont("Liberation Sans");

TempoMarkerPainter::TempoMarkerPainter(TempoMarker* tempoMarker):
    tempoMarker(tempoMarker)
{
    displayFont.setPixelSize(10);

    setDisplayText();
}

void TempoMarkerPainter::setDisplayText()
{
    displayText.clear();

    displayText += QString::fromStdString(tempoMarker->GetDescription());
    displayText += " ";
    displayText += QString().setNum(tempoMarker->GetBeatsPerMinute());

    boundingRectangle = QFontMetricsF(displayFont).boundingRect(displayText);
}

QRectF TempoMarkerPainter::boundingRect() const
{
    return boundingRectangle;
}

void TempoMarkerPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->setFont(displayFont);

    painter->drawText(0, boundingRectangle.height(), displayText);
}
