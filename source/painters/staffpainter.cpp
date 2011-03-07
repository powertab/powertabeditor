#include "staffpainter.h"

#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>
#include <powertabdocument/score.h>

// For manipulating the caret
#include <powertabeditor.h>
#include <scorearea.h>
#include <painters/caret.h>

#include <cmath>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

StaffPainter::StaffPainter(System* system, Staff* staff, const StaffData& staffInfo) :
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
    drawStaffLines(staffInfo.numOfStdNotationLines, staffInfo.stdNotationLineSpacing, staffInfo.getTopStdNotationLine(false));
    // Tab staff
    drawStaffLines(staffInfo.numOfStrings, staffInfo.tabLineSpacing, staffInfo.getTopTabLine(false));
}

void StaffPainter::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    selectionStart = selectionEnd = 0;

    qreal y = mouseEvent->pos().y();
    qreal x = mouseEvent->pos().x();

    // find the position relative to the top of the staff, in terms of the tab line spacing
    // Then, round to find the string index (keep it zero-based since that's what the caret uses)
    int string = findClosestPosition(y, staffInfo.getTopTabLine(false), staffInfo.tabLineSpacing);

    if (string >= 0 && string < staffInfo.numOfStrings)
    {
        Caret* caret = PowerTabEditor::getCurrentScoreArea()->getCaret();

        int position = findClosestPosition(x, system->GetFirstPositionX() + 0.5 * staffInfo.positionWidth, staffInfo.positionWidth) - 1;
        int staffIndex = system->FindStaffIndex(staff);

        Score* currentScore = caret->getCurrentScore();
        int systemIndex = currentScore->FindSystemIndex(system);

        caret->setCurrentSystemIndex(systemIndex);
        caret->setCurrentStaffIndex(staffIndex);
        caret->setCurrentPositionIndex(position);
        caret->setCurrentStringIndex(string);

        selectionStart = selectionEnd = position;
        qDebug() << "Selection Start: " << selectionStart;
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
    int newPos = findClosestPosition(x, system->GetFirstPositionX() + 0.5 * staffInfo.positionWidth, staffInfo.positionWidth);

    if (newPos <= 0 || newPos >= system->GetPositionCount())
        return;

    selectionEnd = newPos;
    if (selectionEnd <= selectionStart)
        selectionEnd--;

    update(boundingRect()); // trigger a redraw
    qDebug() << "Selection End: " << selectionEnd;
}

void StaffPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen);

    painter->drawPath(path);

    painter->setBrush(QColor(168, 205, 241, 125));
    painter->setPen(QPen(QBrush(), 0));

    int leftPos = system->GetPositionX(std::min(selectionStart, selectionEnd));
    int rightPos = system->GetPositionX(std::max(selectionStart, selectionEnd));
    if (selectionEnd < selectionStart)
        rightPos += staffInfo.positionWidth;

    QRectF rect(leftPos, staffInfo.getTopTabLine(false) + 1, rightPos - leftPos, staffInfo.getTabStaffSize() - 1);
    painter->drawRect(rect);
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
