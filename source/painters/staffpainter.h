#ifndef STAFFPAINTER_H
#define STAFFPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

#include <QPen>

class Staff;
class System;

class StaffPainter : public PainterBase
{
public:
    StaffPainter(System* system, Staff* staff, const StaffData& staffInfo);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int drawStaffLines(int lineCount, int lineSpacing, int startHeight);
    inline int findClosestPosition(qreal click, qreal relativePos, qreal spacing);

    System* system;
    Staff* staff;
    StaffData staffInfo;
    QPen pen;

    void init();
};

#endif // STAFFPAINTER_H
