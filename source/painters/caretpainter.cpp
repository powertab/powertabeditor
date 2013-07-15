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

#if 0
    paintSelection(painter);
#endif
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
}

#if 0
Caret::Caret(int tabLineSpacing) :
    inPlaybackMode(false),
    currentScore(NULL),
    currentSystemIndex(0),
    currentStaffIndex(0),
    currentStringIndex(0),
    currentPositionIndex(0),
    currentVoice(0),
    currentStaffTopEdge(0),
    lineSpacing(tabLineSpacing),
    selectionRange(std::make_pair(0, 0))
{
}

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

void Caret::setScore(Score* newScore)
{
    currentScore = newScore;
}

Score* Caret::getCurrentScore() const
{
    return currentScore;
}

QRectF Caret::boundingRect() const
{
    return QRectF(0, -CARET_NOTE_SPACING, getCurrentSystem()->GetRect().GetWidth(),
                  currentStaffInfo.getTabStaffSize() + 2 * CARET_NOTE_SPACING);
}

void Caret::updateStaffInfo()
{
    shared_ptr<const Staff> currentStaff = getCurrentStaff();
    shared_ptr<const System> currentSystem = getCurrentSystem();

    currentStaffInfo.numOfStrings = currentStaff->GetTablatureStaffType();
    currentStaffInfo.stdNotationStaffAboveSpacing = currentStaff->GetStandardNotationStaffAboveSpacing();
    currentStaffInfo.stdNotationStaffBelowSpacing = currentStaff->GetStandardNotationStaffBelowSpacing();
    currentStaffInfo.symbolSpacing = currentStaff->GetSymbolSpacing();
    currentStaffInfo.tabLineSpacing = lineSpacing;
    currentStaffInfo.tabStaffBelowSpacing = currentStaff->GetTablatureStaffBelowSpacing();
    currentStaffInfo.topEdge = currentSystem->GetRect().GetTop();
    currentStaffInfo.leftEdge = currentSystem->GetRect().GetLeft();
    currentStaffInfo.width = currentSystem->GetRect().GetWidth();
    currentStaffInfo.positionWidth = currentSystem->GetPositionSpacing();
    currentStaffInfo.calculateHeight();
}

/// Draws a highlighted box around the selected range
void Caret::paintSelection(QPainter* painter)
{
    painter->setBrush(QColor(168, 205, 241, 125));
    painter->setPen(QPen(QBrush(), 0));

    shared_ptr<System> system = getCurrentSystem();

    int leftPos = system->GetPositionX(std::min(selectionRange.first, selectionRange.second));
    int rightPos = system->GetPositionX(std::max(selectionRange.first, selectionRange.second));

    if (selectionRange.first != selectionRange.second)
    {
        rightPos += currentStaffInfo.positionWidth;
    }

    QRectF rect(leftPos, 1, rightPos - leftPos, currentStaffInfo.getTabStaffSize() - 1);
    painter->drawRect(rect);
}

QString Caret::toString() const
{
	// Add 1 to all indices, since non-computer scientists don't count from 0
    boost::format output = boost::format("System: %d, Staff: %d, Position: %d, String: %d") % (currentSystemIndex + 1)
            % (currentStaffIndex + 1) % (currentPositionIndex + 1) % (currentStringIndex + 1);

    return QString::fromStdString(output.str());
}

void Caret::updatePosition()
{
    shared_ptr<const System> currentSystem = getCurrentSystem();

    updateStaffInfo();
    setPos(currentSystem->GetRect().GetLeft(),
           currentSystem->GetStaffHeightOffset(currentStaffIndex, true) + currentStaffInfo.getTabStaffOffset());
    emit moved();

    // useful debug information - display the caret's location in the widget's tooltip
    setToolTip(this->toString());
}

// Moves the caret either left or right
bool Caret::moveCaretHorizontal(int offset)
{
    const quint32 nextPosition = currentPositionIndex + offset;
    if (getCurrentSystem()->IsValidPosition(nextPosition)) // check that the next position is valid
    {
        setCurrentPositionIndex(nextPosition);
        return true;
    }
    return false;
}

bool Caret::setCurrentStringIndex(uint8_t stringIndex)
{
    if (stringIndex < currentStaffInfo.numOfStrings)
    {
        currentStringIndex = stringIndex;
        update(boundingRect());
        emit moved();
        return true;
    }
    else
    {
        return false;
    }
}

bool Caret::setCurrentSystemIndex(uint32_t systemIndex)
{
    if (currentScore->IsValidSystemIndex(systemIndex))
    {
        currentSystemIndex = systemIndex;
        updatePosition();
        update(boundingRect());
        return true;
    }
    else
    {
        return false;
    }
}

bool Caret::setCurrentStaffIndex(uint32_t staffIndex)
{
    if (getCurrentSystem()->IsValidStaffIndex(staffIndex))
    {
        currentStaffIndex = staffIndex;
        updatePosition();
        update(boundingRect());
        return true;
    }
    else
    {
        return false;
    }
}

bool Caret::setCurrentPositionIndex(uint8_t positionIndex)
{
    if (positionIndex < getCurrentSystem()->GetPositionCount())
    {
        currentPositionIndex = positionIndex;
        selectionRange = std::make_pair(positionIndex, positionIndex);
        updatePosition();
        update(boundingRect());
        return true;
    }
    else
    {
        return false;
    }
}

// Moves the caret up or down
void Caret::moveCaretVertical(int offset)
{
    currentStringIndex = (currentStringIndex + offset + currentStaffInfo.numOfStrings) % currentStaffInfo.numOfStrings;
    update(boundingRect()); // trigger a re-draw
    emit moved();
}

// Moves the caret to the first position in the staff
void Caret::moveCaretToStart()
{
    moveCaretHorizontal( -((int)currentPositionIndex) );
}

// Moves the caret to the last position in the staff
void Caret::moveCaretToEnd()
{
    moveCaretHorizontal( getCurrentSystem()->GetPositionCount() - currentPositionIndex - 1 );
}

bool Caret::moveCaretStaff(int offset)
{
    const quint32 nextStaff = currentStaffIndex + offset;

    if (getCurrentSystem()->IsValidStaffIndex(nextStaff))
    {
        currentStaffIndex = nextStaff;
        selectionRange = std::make_pair(0, 0);

        updatePosition();

        return true;
    }

    return false;
}

bool Caret::moveCaretSection(int offset)
{
    const quint32 nextSystem = currentSystemIndex + offset;
    if (currentScore->IsValidSystemIndex(nextSystem))
    {
        currentStringIndex = 0;
        currentPositionIndex = 0;
        currentStaffIndex = 0;
        selectionRange = std::make_pair(0, 0);
        currentSystemIndex = nextSystem;

        updatePosition();
        return true;
    }
    return false;
}

void Caret::moveCaretToFirstSection()
{
    moveCaretSection( -((int)currentSystemIndex) );
}

void Caret::moveCaretToLastSection()
{
    moveCaretSection( currentScore->GetSystemCount() - currentSystemIndex - 1 );
}

void Caret::moveCaretToNextBar()
{
    System::BarlineConstPtr nextBarline = getCurrentSystem()->GetNextBarline(currentPositionIndex);
    
    // if we are at the end of the system go to next staff
    if (nextBarline == getCurrentSystem()->GetEndBar())
    {
        // if we can't move to the next staff, move to next section
        if (!moveCaretStaff(1))
            moveCaretSection(1);
    }
    else
    {
        // move caret to the first note position of the next bar
        moveCaretHorizontal
                (nextBarline->GetPosition() - currentPositionIndex + 1);
    }
}

void Caret::moveCaretToPrevBar()
{
    shared_ptr<System> currentSystem = getCurrentSystem();
    System::BarlineConstPtr prevBarline = currentSystem->GetPrecedingBarline(currentPositionIndex);

    // if we are at the beginning of the system back up one
    if (prevBarline == currentSystem->GetStartBar())
    {
        // move caret up one staff or section
        if (moveCaretStaff(-1) || moveCaretSection(-1))
        {
            // if new system has > 1 bar, and we actually moved up one
            // (i.e. we are not already at the very first system), move 
            // caret to beginning of last bar
            shared_ptr<const System> newCurrentSystem = getCurrentSystem();
            if (newCurrentSystem->GetBarlineCount() > 0)
            {
                System::BarlineConstPtr lastBar = newCurrentSystem->GetBarline(newCurrentSystem->GetBarlineCount() - 1);

                moveCaretHorizontal(lastBar->GetPosition() - currentPositionIndex + 1);
            }
        }
    }
    else
    {
        // if we need to move to beginning of system
        if (prevBarline == currentSystem->GetBarline(0))
        {
            moveCaretToStart();
            return;
        }
    
        std::vector<System::BarlineConstPtr> sysBars;
        currentSystem->GetBarlines(sysBars);
        
        // we want the position of the bar line before this one
        std::vector<System::BarlineConstPtr>::const_iterator barlinesIt = std::find(sysBars.begin(),
                                                                                    sysBars.end(), prevBarline);
        --barlinesIt;
        
        moveCaretHorizontal
                (((int) (*barlinesIt)->GetPosition()) - 
                 ((int) currentPositionIndex) + 1);
    }
}

shared_ptr<System> Caret::getCurrentSystem() const
{
    return currentScore->GetSystem(currentSystemIndex);
}

shared_ptr<Staff> Caret::getCurrentStaff() const
{
    return getCurrentSystem()->GetStaff(currentStaffIndex);
}

/// may return NULL if the current position does not have a Position object
Position* Caret::getCurrentPosition() const
{
    return getCurrentStaff()->GetPositionByPosition(currentVoice, currentPositionIndex);
}

Note* Caret::getCurrentNote() const
{
    Position* currentPosition = getCurrentPosition();
    if (currentPosition != NULL)
    {
        return currentPosition->GetNoteByString(currentStringIndex);
    }
    else
    {
        return NULL;
    }
}

/// Returns the barline at the current position (or NULL if the current position is not a barline)
boost::shared_ptr<Barline> Caret::getCurrentBarline() const
{
    return getCurrentSystem()->GetBarlineAtPosition(currentPositionIndex);
}

void Caret::updateSelection(int start, int end)
{
#if 0
    qDebug() << "Selected Range: " << start << ", " << end;
#endif
    selectionRange = std::make_pair(start, end);
    emit selectionChanged();
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

/// Returns the index of the voice that is currently active
uint32_t Caret::getCurrentVoice() const
{
    return currentVoice;
}

bool Caret::setCurrentVoice(uint32_t voice)
{
    PTB_CHECK_THAT(Staff::IsValidVoice(voice), false);

    currentVoice = voice;
    return true;
}
#endif
