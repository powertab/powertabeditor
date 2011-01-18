#ifndef STAFFPAINTER_H
#define STAFFPAINTER_H

#include "painterbase.h"

#include <QPen>

class Staff;
class System;

class StaffPainter : public PainterBase
{
public:
    StaffPainter(System* system, Staff* staff, int tabLineSpacing);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int drawStaffLines(QPainter* painter, int lineCount, int lineSpacing, int startHeight);

    System* system;
    Staff* staff;
    QPen pen;
    int tabLineSpacing;
};

#endif // STAFFPAINTER_H
