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
  
#include "barlinepainter.h"

#include "scoreclickevent.h"

#include <QCoreApplication>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <score/barline.h>

const double BarlinePainter::DOUBLE_BAR_WIDTH = 4;

BarlinePainter::BarlinePainter(const LayoutConstPtr &layout,
                               const Barline &barline,
                               const ConstScoreLocation &location,
                               const ScoreClickEvent &click_event,
                               const QColor &barlineColor)
    : ClickableItem(
          QCoreApplication::translate("ScoreArea", "Click to edit barline."),
          click_event, location, ScoreItem::Barline),
      myLayout(layout),
      myBarline(barline),
      myX(0),
      myWidth(0),
      myBarlineColor(barlineColor)
{
    switch (barline.getBarType())
    {
    case Barline::SingleBar:
        myWidth = 1;
        break;
    case Barline::RepeatStart:
        myWidth = -DOUBLE_BAR_WIDTH;
        break;
    default:
        myWidth = DOUBLE_BAR_WIDTH;
        break;
    }

    myX = LayoutInfo::centerItem(myX, myX + layout->getPositionSpacing(),
                                 myWidth);

    // Adjust alignment for repeat barlines.
    if (barline.getBarType() == Barline::RepeatStart ||
        barline.getBarType() == Barline::RepeatEnd)
    {
        myX += myWidth;
    }
    // Adjust for double barlines.
    else if (barline.getBarType() == Barline::DoubleBar ||
             barline.getBarType() == Barline::DoubleBarFine)
    {
        myX -= 2;
    }

    myBounds = QRectF(0, 0, layout->getPositionSpacing(),
                      layout->getStaffHeight());
}

bool
BarlinePainter::filterMousePosition(const QPointF &pos) const
{
    // Only allow clicking within the standard notation staff.
    const double y = pos.y();
    return (y <= myLayout->getBottomStdNotationLine()) &&
           (y >= myLayout->getTopStdNotationLine());
}

void
BarlinePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    ClickableItem::paint(painter, option, widget);

    painter->setPen(QPen(myBarlineColor, 1));
    painter->setBrush(myBarlineColor);

    const Barline::BarType barType = myBarline.getBarType();

    // For the start bar, draw a line connecting the staves.
    if (myBarline.getPosition() == 0 && barType == Barline::SingleBar)
    {
        painter->drawLine(QLineF(myX, myLayout->getBottomStdNotationLine(), myX,
                                 myLayout->getTopTabLine()));
    }

    if (barType == Barline::FreeTimeBar)
        painter->setPen(Qt::DashLine);

    // Print the repeat count for repeat end bars.
    if (barType == Barline::RepeatEnd &&
        myBarline.getRepeatCount() > Barline::MIN_REPEAT_COUNT)
    {
        QFont repeatFont(QStringLiteral("Liberation Sans"));
        repeatFont.setPixelSize(8);
        painter->setFont(repeatFont);

        const QString message = QString::number(myBarline.getRepeatCount()) + "x";
        painter->drawText(3, myLayout->getTopStdNotationLine() - 3, message);
    }

    // Draw a single bar line.
    drawVerticalLines(painter, myX);

    // Draw a second line depending on the bar type.
    if (barType == Barline::DoubleBar || barType == Barline::DoubleBarFine ||
        barType == Barline::RepeatEnd || barType == Barline::RepeatStart)
    {
        // Make the line thicker for certain bar types.
        if (barType != Barline::DoubleBar)
            painter->setPen(QPen(myBarlineColor, 2));

        // Draw the second barline with an offset of the specified width.
        drawVerticalLines(painter, myX + myWidth);
    }

    // Draw the dots for repeats.
    if (barType == Barline::RepeatEnd || barType == Barline::RepeatStart)
    {
        painter->setPen(QPen(myBarlineColor, 0.75));
        const double radius = 1.0;
        // x-coordinate for the location of the dots.
        const double dotLocation = myX - 1.5 * myWidth;
        double height = 0;
        // Middle line for std. notation staff.
        const double centreStaffLine = 3;

        // Draw dots for standard notation staff, on either side of the centre.
        height = (myLayout->getStdNotationLine(centreStaffLine) +
                  myLayout->getStdNotationLine(centreStaffLine + 1)) / 2.0;
        painter->drawRect(QRectF(dotLocation, height, radius, radius));

        height = (myLayout->getStdNotationLine(centreStaffLine) +
                  myLayout->getStdNotationLine(centreStaffLine - 1)) / 2.0;
        painter->drawRect(QRectF(dotLocation, height, radius, radius));

        // Offset the repeat dots 2 lines from the edge of the tab staff if
        // we have a large number of strings, otherwise, only offset by 1 line.
        const int offsetFromEdge = (myLayout->getStringCount() > 4) ? 2 : 1;

        // Draw dots for tab staff.
        height = (myLayout->getTabLine(offsetFromEdge) +
                  myLayout->getTabLine(offsetFromEdge + 1)) / 2.0;
        painter->drawRect(QRectF(dotLocation, height, radius, radius));

        height = (myLayout->getTabLine(myLayout->getStringCount() - offsetFromEdge) +
                  myLayout->getTabLine(myLayout->getStringCount() - offsetFromEdge + 1)) / 2.0;
        painter->drawRect(QRectF(dotLocation, height, radius, radius));
    }
}

void BarlinePainter::drawVerticalLines(QPainter *painter, double x)
{
    QVector<QLineF> lines(2);

    // Draw a single bar line.
    lines[0] = QLineF(x, myLayout->getTopStdNotationLine() + 1,
                      x, myLayout->getBottomStdNotationLine());
    lines[1] = QLineF(x, myLayout->getTopTabLine() + 1,
                      x, myLayout->getBottomTabLine());

    painter->drawLines(lines);
}
