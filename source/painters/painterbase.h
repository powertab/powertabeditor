#ifndef PAINTERBASE_H
#define PAINTERBASE_H

#include <QGraphicsItem>

// Base class for all painters - these classes handle the rendering of
// the score, and also handle interaction (mouse clicks, etc)
//
// This base class is used to provide any common functionality to be shared between all painters
//
// All derived classes must implement the functions paint, boundingRect, mousePressEvent,
// mouseMoveEvent, and mouseReleaseEvent, as they derive from QGraphicsItem.

class PainterBase : public QGraphicsItem
{
public:
    virtual QRectF boundingRect() const = 0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) = 0;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) = 0;

    double CenterItem(double xmin, double xmax, double width)
    {
        return (xmin + ((xmax - (xmin + width)) / 2) + 1);
    }
};

#endif // PAINTERBASE_H
