#include "tabnotepainter.h"

#include <QPainter>
#include <QMessageBox>

#include "../powertabdocument/note.h"

QFontDatabase TabNotePainter::fontDatabase;
QFont TabNotePainter::tabFont = TabNotePainter::fontDatabase.font("Liberation Sans", "", 7.5);
QFontMetricsF TabNotePainter::fontMetrics = QFontMetricsF(tabFont);

TabNotePainter::TabNotePainter(Note* notePtr)
{
    tabFont.setStyleStrategy(QFont::PreferAntialias);
    note = notePtr;
}

void TabNotePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void TabNotePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    QMessageBox message;
    message.setText("Fret: " + QString().setNum(note->GetFretNumber()) + " String: " + QString().setNum(note->GetString()));
    message.exec();
}

void TabNotePainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF TabNotePainter::boundingRect() const
{
    return fontMetrics.boundingRect(QString().setNum(note->GetFretNumber()));
}

void TabNotePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(tabFont);
    painter->drawText(0, 0, QString().setNum(note->GetFretNumber()));
}
