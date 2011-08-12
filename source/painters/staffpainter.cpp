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

#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>
#include <powertabdocument/score.h>

// For manipulating the caret
#include <app/powertabeditor.h>
#include <app/scorearea.h>
#include <painters/caret.h>

#include <cmath>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

StaffPainter::StaffPainter(std::shared_ptr<const System> system, std::shared_ptr<const Staff> staff,
                           const StaffData& staffInfo) :
    system(system),
    staff(staff),
    staffInfo(staffInfo)
{
    pen = QPen(QBrush(QColor(0,0,0)), 0.75);
    init();
}

void StaffPainter::init()
{
    selectionEnd = selectionStart = 0;

    bounds = QRectF(0, 0, staffInfo.width, staffInfo.height);

    // Standard notation staff
    drawStaffLines(staffInfo.numOfStdNotationLines, Staff::STD_NOTATION_LINE_SPACING, staffInfo.getTopStdNotationLine());
    // Tab staff
    drawStaffLines(staffInfo.numOfStrings, staffInfo.tabLineSpacing, staffInfo.getTopTabLine());
}

void StaffPainter::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    qreal y = mouseEvent->pos().y();
    qreal x = mouseEvent->pos().x();

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

void StaffPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    Q_UNUSED(mouseEvent);

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

void StaffPainter::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    const double x = mouseEvent->pos().x();
    selectionEnd = system->GetPositionFromX(x);

    update(boundingRect()); // trigger a redraw
    emit selectionUpdated(selectionStart, selectionEnd);
}

void StaffPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen);

    painter->drawPath(path);
}

int StaffPainter::drawStaffLines(int lineCount, int lineSpacing, int startHeight)
{
    int height = 0;

    for (int i=0; i < lineCount; i++)
    {
        height = i * lineSpacing + startHeight;
        path.moveTo(0, height);
        path.lineTo(staffInfo.width, height);
    }

    return height;
}
