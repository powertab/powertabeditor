#include "directionpainter.h"

#include <QPainter>
#include <QFontMetricsF>
#include <powertabdocument/direction.h>

QFont DirectionPainter::displayFont = QFont("Liberation Sans");

DirectionPainter::DirectionPainter(const Direction* direction) :
    direction(direction)
{
    displayFont.setPixelSize(10);
    displayFont.setItalic(true);
    displayFont.setStyleStrategy(QFont::PreferAntialias);

    init();
}

void DirectionPainter::init()
{
    const QString text = QString::fromStdString(direction->GetText(0));

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    QFontMetricsF fm(displayFont);
    bounds = QRectF(0, 0, fm.width(text), fm.height());
}

void DirectionPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(displayFont);
    painter->drawStaticText(0, 0, displayText);
}
