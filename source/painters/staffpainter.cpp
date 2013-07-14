/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "staffpainter.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

StaffPainter::StaffPainter(const LayoutConstPtr &layout)
    : myLayout(layout),
      myBounds(0, 0, LayoutInfo::STAFF_WIDTH, layout->getStaffHeight())
{
#if 0
    selectionEnd = selectionStart = 0;
#endif

    // Only use the left mouse button for making selections.
    setAcceptedMouseButtons(Qt::LeftButton);
}

#if 0
void StaffPainter::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qreal y = event->pos().y();
    qreal x = event->pos().x();

    // find the position relative to the top of the staff, in terms of the tab line spacing
    // Then, round to find the string index (keep it zero-based since that's what the caret uses)
    int string = findClosestPosition(y, staffInfo.getTopTabLine(), staffInfo.tabLineSpacing);

    if (string >= 0 && string < staffInfo.numOfStrings)
    {
        Caret* caret = PowerTabEditor::getCurrentScoreArea()->getCaret();

        const int position = system->GetPositionFromX(x);

        int staffIndex = system->FindStaffIndex(staff);

        Score* currentScore = caret->getCurrentScore();
        int systemIndex = currentScore->FindSystemIndex(system);

        caret->setCurrentSystemIndex(systemIndex);
        caret->setCurrentStaffIndex(staffIndex);
        caret->setCurrentPositionIndex(position);
        caret->setCurrentStringIndex(string);

        selectionStart = selectionEnd = position;
        emit selectionUpdated(selectionStart, selectionEnd);
    }
}

void StaffPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    update(boundingRect());
}

/// useful function for figuring out what string and what position a mouse click occurred at
/// Finds the closest position, using the spacing between each position and the start point
/// @param click - The position that the click occurred at
/// @param relativePos - The position that everything is relative to (i.e. top line of a staff)
/// @param spacing - Spacing between items
inline int StaffPainter::findClosestPosition(qreal click, qreal relativePos, qreal spacing)
{
    qreal temp = (click - relativePos) / spacing;
    int pos = floor(temp + 0.5);
    return pos;
}

void StaffPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    const double x = event->pos().x();
    selectionEnd = system->GetPositionFromX(x);

    update(boundingRect()); // trigger a redraw
    emit selectionUpdated(selectionStart, selectionEnd);
}
#endif

void StaffPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                         QWidget *)
{
    painter->setPen(QPen(QBrush(QColor(0,0,0)), 0.75));

    // Draw standard notation staff.
    drawStaffLines(painter, LayoutInfo::NUM_STD_NOTATION_LINES,
                   myLayout->STD_NOTATION_LINE_SPACING,
                   myLayout->getTopStdNotationLine());

    // Draw tab staff.
    drawStaffLines(painter, myLayout->getStringCount(),
                   myLayout->getTabLineSpacing(), myLayout->getTopTabLine());
}

void StaffPainter::drawStaffLines(QPainter *painter, int lineCount,
                                  double lineSpacing, double startHeight)
{
    for (int i = 0; i < lineCount; i++)
    {
        const double height = i * lineSpacing + startHeight;
        painter->drawLine(QPointF(0, height),
                          QPointF(LayoutInfo::STAFF_WIDTH, height));
    }
}
