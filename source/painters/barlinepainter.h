#ifndef BARLINEPAINTER_H
#define BARLINEPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

class Barline;

class BarlinePainter : public PainterBase
{
public:
    BarlinePainter(StaffData staffInformation, Barline* barLinePtr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void init();

private:
    StaffData staffInfo;
    Barline* barLine;
    double x;
    double width;

    static const double DOUBLE_BAR_WIDTH;
};

#endif // BARLINEPAINTER_H
