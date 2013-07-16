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
#include <boost/lexical_cast.hpp>
#include <painters/layoutinfo.h>
#include <QDebug>
#include <QPainter>
#include <score/scorelocation.h>
#include <score/system.h>

const double CaretPainter::PEN_WIDTH = 0.75;
const double CaretPainter::CARET_NOTE_SPACING = 6;

CaretPainter::CaretPainter(const Caret &caret)
    : myCaret(caret)
{
    caret.subscribeToChanges(boost::bind(&CaretPainter::onLocationChanged,
                                         this));
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

#if 0
    // set color
    if (inPlaybackMode)
    {
        painter->setPen(QPen(Qt::red, PEN_WIDTH));
    }
    else
    {
        painter->setPen(QPen(Qt::blue, PEN_WIDTH));
    }
#else
    painter->setPen(QPen(Qt::blue, PEN_WIDTH));
#endif

    double left = myLayout->getPositionX(location.getPositionIndex());
    const double y1 = 0;
    const double y2 = myLayout->getTabStaffHeight();
    const double x = LayoutInfo::centerItem(left,
                                            left + myLayout->getPositionSpacing(),
                                            1);

#if 0
    // If in playback mode, just draw a vertical line and don't highlight
    // the selected note.
    if (inPlaybackMode)
    {
        painter->drawLine(x, y1, x, y2);
    }
    else
#endif
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

void CaretPainter::onLocationChanged()
{
    const ScoreLocation &location = myCaret.getLocation();
    const System &system = location.getSystem();
    myLayout.reset(new LayoutInfo(system, location.getStaff()));

    // Compute the offset due to the previous systems.
    double offset = 0;
    for (int i = 0; i < location.getStaffIndex(); ++i)
        offset += LayoutInfo(system, system.getStaves()[i]).getStaffHeight();

    update(boundingRect());
    setPos(0, mySystemRects.at(location.getSystemIndex()).top() + offset +
           myLayout->getSystemSymbolSpacing() + myLayout->getStaffHeight() -
           myLayout->getTabStaffBelowSpacing() - myLayout->STAFF_BORDER_SPACING -
           myLayout->getTabStaffHeight());

    setToolTip(QString::fromStdString(
                   boost::lexical_cast<std::string>(location)));
}

#if 0
void Caret::setPlaybackMode(bool playBack)
{
    inPlaybackMode = playBack;
    update(boundingRect()); // redraw the caret
}

bool Caret::isInPlaybackMode() const
{
    return inPlaybackMode;
}

/// Adjusts the caret's location to be valid (useful after e.g. deleting a
/// system).
void Caret::adjustToValidLocation()
{
    using namespace Common;

    currentSystemIndex = clamp<uint32_t>(currentSystemIndex, 0,
                                    getCurrentScore()->GetSystemCount() - 1);
    currentStaffIndex = clamp<uint32_t>(currentStaffIndex, 0,
                                    getCurrentSystem()->GetStaffCount() - 1);

    uint32_t posLimit = getCurrentSystem()->GetPositionCount() - 1;
    currentPositionIndex = clamp<uint32_t>(currentPositionIndex, 0, posLimit);
    currentStringIndex = clamp<uint32_t>(currentStringIndex, 0,
                                         currentStaffInfo.numOfStrings - 1);
    selectionRange.first = clamp<int>(selectionRange.first, 0, posLimit);
    selectionRange.second = clamp<int>(selectionRange.second, 0, posLimit);

    updatePosition();
    update(boundingRect());
}

/// Returns a list of all of the Position objects that are currently selected
std::vector<Position*> Caret::getSelectedPositions() const
{
    std::vector<Position*> positions;
    getCurrentStaff()->GetPositionsInRange(positions, currentVoice,
                                           std::min(selectionRange.first, selectionRange.second),
                                           std::max(selectionRange.first, selectionRange.second));

    return positions;
}

/// Returns a list of all of the Note objects that are currently selected.
std::vector<Note*> Caret::getSelectedNotes() const
{
    std::vector<Note*> notes;

    if (hasSelection())
    {
        const std::vector<Position*> positions = getSelectedPositions();
        for (size_t i = 0; i < positions.size(); i++)
        {
            for (size_t j = 0; j < positions[i]->GetNoteCount(); j++)
            {
                notes.push_back(positions[i]->GetNote(j));
            }
        }
    }
    else
    {
        Note* selectedNote = getCurrentNote();
        if (selectedNote)
        {
            notes.push_back(selectedNote);
        }
    }

    return notes;
}

/// Returns a list of all of the barlines in the current selection.
std::vector<boost::shared_ptr<Barline> > Caret::getSelectedBarlines() const
{
    std::vector<boost::shared_ptr<Barline> > barlineArray;
    getCurrentSystem()->GetBarlinesInRange(barlineArray,
                std::min(selectionRange.first, selectionRange.second),
                std::max(selectionRange.first, selectionRange.second));

    return barlineArray;
}

/// Returns whether there is a selection (regardless of whether any notes,
/// positions, etc are actually selected).
bool Caret::hasSelection() const
{
    return selectionRange.first != selectionRange.second;
}

#endif
