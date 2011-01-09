#include "chordtextpainter.h"

#include <QPainter>
#include <QFontMetricsF>

#include "../powertabdocument/chordtext.h"

QFont ChordTextPainter::displayFont = QFont("Liberation Sans");

ChordTextPainter::ChordTextPainter(ChordText* chordText) :
        chordText(chordText)
{
    displayFont.setPixelSize(10); // needed for cross-platform consistency in font size
    displayFont.setStyleStrategy(QFont::PreferAntialias);
    displayText = "";
}

void ChordTextPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void ChordTextPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
}

void ChordTextPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF ChordTextPainter::boundingRect() const
{
    return QFontMetricsF(displayFont).boundingRect(displayText);
}

void ChordTextPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setFont(displayFont);

    displayText = QString().fromStdString(chordText->GetText());

    painter->drawText(0, 0, displayText);
}
