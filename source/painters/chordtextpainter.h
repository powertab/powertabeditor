#ifndef CHORDTEXTPAINTER_H
#define CHORDTEXTPAINTER_H

#include "painterbase.h"

#include <QFont>

class ChordText;

class ChordTextPainter : public PainterBase
{
public:
    ChordTextPainter(ChordText* chordText);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    ChordText* chordText;
    QString displayText; // holds the display text so that we don't have to recalculate it for boundingRect()
    static QFont displayFont;
};

#endif // CHORDTEXTPAINTER_H
