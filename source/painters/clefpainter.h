#ifndef CLEFPAINTER_H
#define CLEFPAINTER_H

#include "painterbase.h"

#include <QStaticText>
#include <memory>

class StaffData;
class Staff;

class ClefPainter : public PainterBase
{
public:
    ClefPainter(const StaffData& staffData, std::shared_ptr<const Staff> staff);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void init();

    const StaffData& staffInfo;
    std::shared_ptr<const Staff> staff;
    static QFont font;
    QStaticText displayText;
};

#endif // CLEFPAINTER_H
