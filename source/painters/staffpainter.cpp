#include "staffpainter.h"

#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>

#include <QPainter>

StaffPainter::StaffPainter(System* system, Staff* staff, int tabLineSpacing) :
        system(system),
        staff(staff),
        tabLineSpacing(tabLineSpacing)
{
    pen = QPen(QBrush(QColor(0,0,0)), 0.75);
}

void StaffPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void StaffPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
}

void StaffPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF StaffPainter::boundingRect() const
{
    return QRectF(0, 0, system->GetRect().GetWidth(), staff->GetHeight());
}

void StaffPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen);

    // Standard notation staff
    int height = drawStaffLines(painter, 5, staff->STD_NOTATION_LINE_SPACING, staff->GetStandardNotationStaffAboveSpacing() + staff->STAFF_BORDER_SPACING);
    // Tab staff
    drawStaffLines(painter, staff->GetTablatureStaffType(), tabLineSpacing, height + staff->GetSymbolSpacing() + 2 * staff->STAFF_BORDER_SPACING + staff->GetStandardNotationStaffBelowSpacing());

}

int StaffPainter::drawStaffLines(QPainter* painter, int lineCount, int lineSpacing, int startHeight)
{
    const int width = system->GetRect().GetWidth();
    int height;

    for (int i=0; i < lineCount; i++)
    {
        height = i * lineSpacing + startHeight;
        painter->drawLine(0, height, width, height);
    }

    return height;
}
