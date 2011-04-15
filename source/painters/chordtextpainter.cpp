#include "chordtextpainter.h"

#include <QPainter>
#include <QFontMetricsF>

#include <powertabdocument/chordtext.h>

QFont ChordTextPainter::displayFont = QFont("Liberation Sans");

ChordTextPainter::ChordTextPainter(const ChordText* chordText) :
    chordText(chordText)
{
    displayFont.setPixelSize(10); // needed for cross-platform consistency in font size
    displayFont.setStyleStrategy(QFont::PreferAntialias);

    init();
}

void ChordTextPainter::init()
{
    const QString text = QString::fromStdString(chordText->GetText());

    displayText.setText(text);
    displayText.prepare(QTransform(), displayFont);

    QFontMetricsF fm(displayFont);
    bounds = QRectF(0, 0, fm.width(text), fm.height());
}

void ChordTextPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(displayFont);

    painter->drawStaticText(0, 0, displayText);
}
