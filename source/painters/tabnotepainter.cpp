#include "tabnotepainter.h"

#include <QPainter>
#include <QFontMetricsF>

#include <powertabdocument/note.h>

QFont TabNotePainter::tabFont = QFont("Liberation Sans");

TabNotePainter::TabNotePainter(Note* note) :
    note(note)
{
    tabFont.setPixelSize(10); // needed for cross-platform consistency in font size
    tabFont.setStyleStrategy(QFont::PreferAntialias);

    init();
}

void TabNotePainter::init()
{
    QString noteText = QString().fromStdString(note->GetText());

    displayText.setText(noteText);
    displayText.prepare(QTransform(), tabFont);

    QFontMetricsF fm(tabFont);
    bounds = QRectF(0, 0, fm.width(noteText), fm.height());
}

void TabNotePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(tabFont);

    if(note->IsTied())
    {
        painter->setPen(Qt::lightGray);
    }
    else
    {
        painter->setPen(Qt::black);
    }

    // offset height by 1 pixel for clarity
    painter->drawStaticText(0, 0, displayText);
}
