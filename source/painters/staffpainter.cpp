#include "staffpainter.h"

#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

StaffPainter::StaffPainter(System* system, Staff* staff, const StaffData& staffInfo) :
        system(system),
        staff(staff),
        staffInfo(staffInfo)
{
    pen = QPen(QBrush(QColor(0,0,0)), 0.75);
}

void StaffPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void StaffPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    qreal y = mouseEvent->pos().y();
    qDebug() << "Mouse click on staff at height: " << y;
}

void StaffPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF StaffPainter::boundingRect() const
{
    return QRectF(0, 0, staffInfo.width, staffInfo.height);
}

void StaffPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen);

    // Standard notation staff
    drawStaffLines(painter, 5, staffInfo.stdNotationLineSpacing, staffInfo.getTopStdNotationLine(false));
    // Tab staff
    drawStaffLines(painter, staffInfo.numOfStrings, staffInfo.tabLineSpacing, staffInfo.getTopTabLine(false));

}

int StaffPainter::drawStaffLines(QPainter* painter, int lineCount, int lineSpacing, int startHeight)
{
    int height;

    for (int i=0; i < lineCount; i++)
    {
        height = i * lineSpacing + startHeight;
        painter->drawLine(0, height, staffInfo.width, height);
    }

    return height;
}
