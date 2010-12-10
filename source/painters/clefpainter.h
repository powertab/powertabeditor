#ifndef CLEFPAINTER_H
#define CLEFPAINTER_H

#include "painterbase.h"

class StaffData;
class Staff;

class ClefPainter : public PainterBase
{
public:
    ClefPainter(const StaffData& staffData, Staff* staff);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    const StaffData& staffInfo;
    Staff* staff;
};

#endif // CLEFPAINTER_H
