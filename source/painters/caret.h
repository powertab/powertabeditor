#ifndef CARET_H
#define CARET_H

#include "painterbase.h"
#include "staffdata.h"

class Note;
class Position;
class Staff;
class System;
class Score;

// Highlights the current position within the score

class Caret : public PainterBase
{
public:
    Caret(int tabLineSpacing);

    void updatePosition();

    void moveCaretHorizontal(int offset);
    void moveCaretVertical(int offset);
    void moveCaretToStart();
    void moveCaretToEnd();

    void moveCaretSection(int offset);
    void moveCaretToFirstSection();
    void moveCaretToLastSection();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    void setScore(Score* newScore);
    void setSystem(System* newSystem);
    void setStaff(Staff* newStaff);
    void setPosition(Position* newPosition);
    void setNote(Note* newNote);

    Score* getCurrentScore()
    {
        return currentScore;
    }

    System* getCurrentSystem()
    {
        return currentSystem;
    }

    Staff* getCurrentStaff()
    {
        return currentStaff;
    }

    Position* getCurrentPosition()
    {
        return selectedPosition;
    }

	quint32 getCurrentPositionIndex()
	{
		return currentPositionIndex;
	}

    Note* getCurrentNote()
    {
        return selectedNote;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void updateStaffInfo();

private:
    quint32 currentSectionIndex;
    quint32 currentStringIndex;
    quint32 currentPositionIndex;
    int lineSpacing;
    Score* currentScore;
    System* currentSystem;
    Staff* currentStaff;
    Note* selectedNote;
    Position* selectedPosition;
    StaffData currentStaffInfo;

    static const int CARET_NOTE_SPACING = 6; // spacing around a highlighted note
};

#endif // CARET_H
