#ifndef TIMESIGNATUREPAINTER_H
#define TIMESIGNATUREPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

class StaffData;
class TimeSignature;

class TimeSignaturePainter : public PainterBase
{
public:
    TimeSignaturePainter(const StaffData& staffInformation, const TimeSignature& signature);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void init();
    void drawNumber(QPainter* painter, const double y, const quint8 number) const;

    StaffData staffInfo;
    const TimeSignature& timeSignature;
};

#endif // TIMESIGNATUREPAINTER_H
