#include "chordtextpainter.h"

#include <QPainter>
#include <QFontMetricsF>

#include <powertabdocument/chordtext.h>

QFont ChordTextPainter::displayFont = QFont("Liberation Sans");

ChordTextPainter::ChordTextPainter(ChordText* chordText) :
    chordText(chordText)
{
    displayFont.setPixelSize(10); // needed for cross-platform consistency in font size
    displayFont.setStyleStrategy(QFont::PreferAntialias);

    init();
}

void ChordTextPainter::init()
{
    QString text = QString::fromStdString(chordText->GetText());

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    QFontMetricsF fm(displayFont);
    bounds = QRectF(0, 0, fm.width(text), fm.height());
}

void ChordTextPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    painter->setFont(displayFont);

    painter->drawStaticText(0, 0, displayText);
}
