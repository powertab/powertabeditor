#include "caret.h"

#include <QPainter>
#include <QVector>

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/note.h>
#include <powertabdocument/position.h>

Caret::Caret(int tabLineSpacing)
{
    lineSpacing = tabLineSpacing;
    currentScore = NULL;
    currentSystem = NULL;
    currentStaff = NULL;
    selectedNote = NULL;
    selectedPosition = NULL;

    currentPositionIndex = 0;
    currentStringIndex = 0;
    currentSystemIndex = 0;
    currentStaffIndex = 0;
    currentStaffTopEdge = 0;
    inPlaybackMode = false;
}

void Caret::setPlaybackMode(bool playBack)
{
    inPlaybackMode = playBack;
    update(boundingRect()); // redraw the caret
}

void Caret::setScore(Score* newScore)
{
    currentScore = newScore;
}


void Caret::setSystem(System* newSystem)
{
    currentSystem = newSystem;
}

void Caret::setStaff(Staff* newStaff)
{
    currentStaff = newStaff;
}

void Caret::setPosition(Position* newPosition)
{
    selectedPosition = newPosition;
}

void Caret::setNote(Note* newNote)
{
    selectedNote = newNote;
}

QRectF Caret::boundingRect() const
{
    return QRectF(0, -CARET_NOTE_SPACING, currentSystem->GetPositionSpacing(),
                  currentStaffInfo.getTabStaffSize() + 2 * CARET_NOTE_SPACING);
}

void Caret::updateStaffInfo()
{
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


void Caret::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    updateStaffInfo();

    const double PEN_WIDTH = 0.75;
    // set color
    if (inPlaybackMode)
    {
        painter->setPen(QPen(Qt::red, PEN_WIDTH));
    }
    else
    {
        painter->setPen(QPen(Qt::blue, PEN_WIDTH));
    }

    // get top
    int y1 = 0;
    // get bottom
    int y2 = currentStaffInfo.getTabStaffSize();
    // get x-coordinate
    int x = centerItem(0, currentStaffInfo.positionWidth, 1);

    if (inPlaybackMode) // if in playback mode, just draw a vertical line and don't highlight selected note
    {
        painter->drawLine(x, y1, x, y2);
    }
    else
    {
        QVector<QLine> lines(0);
        // calculations for the box around the selected note
        int stringHeight = currentStaffInfo.getTabLineHeight(currentStringIndex + 1) - currentStaffInfo.getTopTabLine();
        int boundary1 = stringHeight - CARET_NOTE_SPACING;
        int boundary2 = stringHeight + CARET_NOTE_SPACING;

        // draw a line down to the highlighted string, if necessary
        if (y1 < boundary1)
        {
            lines.append(QLine(x, y1, x, boundary1));
        }

        // draw horizontal lines around note
        lines.append(QLine(0, boundary1, currentStaffInfo.positionWidth, boundary1));
        lines.append(QLine(0, boundary2, currentStaffInfo.positionWidth, boundary2));

        // draw to bottom of staff, if necessary
        if (y2 > boundary2)
        {
            lines.append(QLine(x, boundary2, x, y2));
        }
        painter->drawLines(lines);
    }
}

void Caret::updatePosition()
{
    updateStaffInfo();
    setPos(currentSystem->GetPositionX(currentPositionIndex),
           currentSystem->GetStaffHeightOffset(currentStaffIndex, true) + currentStaffInfo.getTabStaffOffset()
           );
    emit moved();
}

// Moves the caret either left or right
bool Caret::moveCaretHorizontal(int offset)
{
    const quint32 nextPosition = currentPositionIndex + offset;
    if (currentSystem->IsValidPosition(nextPosition)) // check that the next position is valid
    {
        currentPositionIndex = nextPosition;
        moveToNewPosition();
        updatePosition(); // redraw the caret
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
        return true;
    }
    else
    {
        return false;
    }
}

bool Caret::setCurrentSystemIndex(uint32_t systemIndex)
{
    if (systemIndex < currentScore->GetSystemCount())
    {
        currentSystemIndex = systemIndex;
        moveToNewPosition();
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
    if (staffIndex < currentSystem->GetStaffCount())
    {
        currentStaffIndex = staffIndex;
        moveToNewPosition();
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
    if (positionIndex < currentSystem->GetPositionCount())
    {
        currentPositionIndex = positionIndex;
        moveToNewPosition();
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
    // TODO - select any note that occurs at this string
}

// Moves the caret to the first position in the staff
void Caret::moveCaretToStart()
{
    moveCaretHorizontal( -((int)currentPositionIndex) );
}

// Moves the caret to the last position in the staff
void Caret::moveCaretToEnd()
{
    moveCaretHorizontal( currentSystem->GetPositionCount() - currentPositionIndex - 1 );
}

bool Caret::moveCaretStaff(int offset)
{
    const quint32 nextStaff = currentStaffIndex + offset;

    if (currentSystem->IsValidStaffIndex(nextStaff))
    {
        currentStaffIndex = nextStaff;
        currentStringIndex = 0;
        currentPositionIndex = 0;

        moveToNewPosition();
        updatePosition();

        return true;
    }

    return false;
}

void Caret::moveToNewPosition()
{
    currentSystem = currentScore->GetSystem(currentSystemIndex);
    currentStaff = currentSystem->GetStaff(currentStaffIndex);
    selectedPosition = currentStaff->GetPositionByPosition(currentPositionIndex);
    selectedNote = 0;
}

bool Caret::moveCaretSection(int offset)
{
    const quint32 nextSystem = currentSystemIndex + offset;
    if (currentScore->IsValidSystemIndex(nextSystem))
    {
        currentStringIndex = 0;
        currentPositionIndex = 0;
        currentStaffIndex = 0;
        currentSystemIndex = nextSystem;

        moveToNewPosition();

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
