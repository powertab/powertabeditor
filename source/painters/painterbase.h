#ifndef PAINTERBASE_H
#define PAINTERBASE_H

#include <QGraphicsItem>

// Base class for all painters - these classes handle the rendering of
// the score, and also handle interaction (mouse clicks, etc)
//
// This base class is used to provide any common functionality to be shared between all painters
//
// All derived classes must implement the functions paint and boundingRect.

class PainterBase : public QGraphicsItem
{
public:
    virtual QRectF boundingRect() const
    {
        return bounds;
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;

protected:

    double centerItem(double xmin, double xmax, double width)
    {
        return (xmin + ((xmax - (xmin + width)) / 2) + 1);
    }

    QRectF bounds;
    QPainterPath path;
};

#endif // PAINTERBASE_H
