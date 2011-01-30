#ifndef CLEFPAINTER_H
#define CLEFPAINTER_H

#include "painterbase.h"

#include <QStaticText>

class StaffData;
class Staff;

class ClefPainter : public PainterBase
{
public:
    ClefPainter(const StaffData& staffData, Staff* staff);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void init();

    const StaffData& staffInfo;
    Staff* staff;
    static QFont font;
    QStaticText displayText;
};

#endif // CLEFPAINTER_H
