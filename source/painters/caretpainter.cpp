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
  
#include "caretpainter.h"

#include <app/caret.h>
#include <app/viewoptions.h>
#include <boost/lexical_cast.hpp>
#include <painters/layoutinfo.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <score/scorelocation.h>
#include <score/score.h>
#include <score/system.h>

const double CaretPainter::PEN_WIDTH = 0.75;
const double CaretPainter::CARET_NOTE_SPACING = 6;

CaretPainter::CaretPainter(const Caret &caret, const ViewOptions &view_options)
    : myCaret(caret),
      myViewOptions(view_options),
      myCaretConnection(caret.subscribeToChanges([=]() {
          onLocationChanged();
      }))
{
}

void CaretPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                         QWidget *)
{
    if (!myLayout)
    {
        onLocationChanged();
        return;
    }

    const ScoreLocation &location = myCaret.getLocation();
    const bool hasFocus = scene()->views().first()->hasFocus();

    // Set color.
    if (!hasFocus)
        painter->setPen(QPen(Qt::darkGray, PEN_WIDTH));
    else if (myCaret.isInPlaybackMode())
        painter->setPen(QPen(Qt::red, PEN_WIDTH));
    else
        painter->setPen(QPen(Qt::blue, PEN_WIDTH));

    double left = myLayout->getPositionX(location.getPositionIndex());
    const double y1 = 0;
    const double y2 = myLayout->getTabStaffHeight();
    const double x = LayoutInfo::centerItem(left,
                                            left + myLayout->getPositionSpacing(),
                                            1);

    // If in playback mode, just draw a vertical line and don't highlight
    // the selected note.
    if (myCaret.isInPlaybackMode())
    {
        painter->drawLine(x, y1, x, y2);
        return;
    }

    QVector<QLine> lines(0);

    // Calculations for the box around the selected note.
    const double stringHeight = myLayout->getTabLine(location.getString() + 1) -
            myLayout->getTopTabLine();
    const double boundary1 = stringHeight - CARET_NOTE_SPACING;
    const double boundary2 = stringHeight + CARET_NOTE_SPACING;

    // Draw a line down to the highlighted string, if necessary.
    if (y1 < boundary1)
        lines.append(QLine(x, y1, x, boundary1));

    // Draw horizontal lines around note, but don't exceed the default width.
    const double width = std::min(myLayout->getPositionSpacing(),
                                  LayoutInfo::DEFAULT_POSITION_SPACING);

    left = LayoutInfo::centerItem(left, left + myLayout->getPositionSpacing(),
                                  width);
    lines.append(QLine(left, boundary1, left + width, boundary1));
    lines.append(QLine(left, boundary2, left + width, boundary2));

    // Draw to bottom of staff, if necessary.
    if (y2 > boundary2)
        lines.append(QLine(x, boundary2, x, y2));

    painter->drawLines(lines);

    // Draw the selection
    painter->setBrush(QColor(168, 205, 241, 125));
    painter->setPen(QPen(QBrush(), 0));

    left = myLayout->getPositionX(std::min(location.getSelectionStart(),
                                           location.getPositionIndex()));
    double right = myLayout->getPositionX(std::max(location.getSelectionStart(),
                                                   location.getPositionIndex()));

    if (left != right)
        right += myLayout->getPositionSpacing();

    painter->drawRect(QRectF(left, 1, right - left,
                             myLayout->getTabStaffHeight() - 1));
}

QRectF CaretPainter::boundingRect() const
{
    if (myLayout)
    {
        return QRectF(0, -CARET_NOTE_SPACING, LayoutInfo::STAFF_WIDTH,
                      myLayout->getTabStaffHeight() + 2 * CARET_NOTE_SPACING);
    }
    else
        return QRectF();
}

void CaretPainter::addSystemRect(const QRectF &rect)
{
    mySystemRects.push_back(rect);
}

void CaretPainter::setSystemRect(int index, const QRectF &rect)
{
    mySystemRects.at(index) = rect;
}

QRectF CaretPainter::getCurrentSystemRect() const
{
    return mySystemRects.at(myCaret.getLocation().getSystemIndex());
}

void CaretPainter::updatePosition()
{
    // Force an update of the caret.
    onLocationChanged();
}

boost::signals2::connection CaretPainter::subscribeToMovement(
        const LocationChangedSlot::slot_type &subscriber)
{
    return onMyLocationChanged.connect(subscriber);
}

void CaretPainter::onLocationChanged()
{
    const ScoreLocation &location = myCaret.getLocation();
    if (location.getScore().getSystems().empty())
        return;

    const System &system = location.getSystem();
    if (system.getStaves().empty())
        return;

    myLayout.reset(new LayoutInfo(location.getScore(), system,
                                  location.getSystemIndex(), location.getStaff(),
                                  location.getStaffIndex()));

    const ViewFilter *filter =
        myViewOptions.getFilter()
            ? &location.getScore().getViewFilters()[*myViewOptions.getFilter()]
            : nullptr;

    // Compute the offset due to the previous (visible) staves.
    double offset = 0;
    for (int i = 0; i < location.getStaffIndex(); ++i)
    {
        if (!filter ||
            filter->accept(location.getScore(), location.getSystemIndex(), i))
        {
            offset += LayoutInfo(location.getScore(), system,
                                 location.getSystemIndex(),
                                 system.getStaves()[i], i).getStaffHeight();
        }
    }

    const QRectF oldRect = sceneBoundingRect();
    setPos(0, mySystemRects.at(location.getSystemIndex()).top() + offset +
           myLayout->getSystemSymbolSpacing() + myLayout->getStaffHeight() -
           myLayout->getTabStaffBelowSpacing() - myLayout->STAFF_BORDER_SPACING -
           myLayout->getTabStaffHeight());
    update(boundingRect());
    // Ensure that a redraw always occurs at the old location.
    scene()->update(oldRect);

    setToolTip(QString::fromStdString(
                   boost::lexical_cast<std::string>(location)));

    // Notify anyone interested in the caret being redrawn.
    onMyLocationChanged();
}
