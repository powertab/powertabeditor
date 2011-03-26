#ifndef STAFFPAINTER_H
#define STAFFPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

#include <memory>
#include <QPen>

class Staff;
class System;

class StaffPainter : public PainterBase
{
public:
    StaffPainter(std::shared_ptr<System> system, Staff* staff, const StaffData& staffInfo);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int drawStaffLines(int lineCount, int lineSpacing, int startHeight);
    inline int findClosestPosition(qreal click, qreal relativePos, qreal spacing);

    std::shared_ptr<System> system;
    Staff* staff;
    StaffData staffInfo;
    QPen pen;

    int selectionStart;
    int selectionEnd;

    void init();
};

#endif // STAFFPAINTER_H
