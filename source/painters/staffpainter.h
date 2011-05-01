#ifndef STAFFPAINTER_H
#define STAFFPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

#include <memory>
#include <QPen>

class Staff;
class System;

class StaffPainter : public QObject, public PainterBase
{
    Q_OBJECT

public:
    StaffPainter(std::shared_ptr<const System> system, std::shared_ptr<const Staff> staff,
                 const StaffData& staffInfo);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void selectionUpdated(int selectionStart, int selectionEnd);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int drawStaffLines(int lineCount, int lineSpacing, int startHeight);
    inline int findClosestPosition(qreal click, qreal relativePos, qreal spacing);

    std::shared_ptr<const System> system;
    std::shared_ptr<const Staff> staff;
    StaffData staffInfo;
    QPen pen;

    int selectionStart;
    int selectionEnd;

    void init();
};

#endif // STAFFPAINTER_H
