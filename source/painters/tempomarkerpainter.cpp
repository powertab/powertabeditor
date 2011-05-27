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
