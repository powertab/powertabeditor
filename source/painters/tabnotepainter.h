#ifndef TABNOTEPAINTER_H
#define TABNOTEPAINTER_H

#include "painterbase.h"

#include <QFontDatabase>
#include <QFontMetricsF>

class Note;

class TabNotePainter : public PainterBase
{
public:
    TabNotePainter(Note* notePtr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    Note* note;
    QFont tabFont;
};

#endif // TABNOTEPAINTER_H
