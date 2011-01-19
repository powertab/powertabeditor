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

class Caret : public QObject, public PainterBase // inheritance from QObject needed for signals/slots
{
    Q_OBJECT

public:
    Caret(int tabLineSpacing);

    void updatePosition();
    void setPlaybackMode(bool playBack);

    bool moveCaretHorizontal(int offset);
    void moveCaretVertical(int offset);
    void moveCaretToStart();
    void moveCaretToEnd();

    bool moveCaretSection(int offset);
    void moveCaretToFirstSection();
    void moveCaretToLastSection();

    bool moveCaretStaff(int offset);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    void setScore(Score* newScore);
    void setSystem(System* newSystem);
    void setStaff(Staff* newStaff);
    void setPosition(Position* newPosition);
    void setNote(Note* newNote);

    bool setCurrentStringIndex(uint8_t stringIndex);
    bool setCurrentPositionIndex(uint8_t positionIndex);
    bool setCurrentStaffIndex(uint32_t staffIndex);
    bool setCurrentSystemIndex(uint32_t systemIndex);

    Score* getCurrentScore()
    {
        return currentScore;
    }

    System* getCurrentSystem()
    {
        return currentSystem;
    }

    quint32 getSystemIndex()
    {
        return currentSystemIndex;
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


signals:
    void moved();

protected:
    void updateStaffInfo();
    void moveToNewPosition();

    bool inPlaybackMode;
    quint32 currentSystemIndex;
    quint32 currentStringIndex;
    quint32 currentPositionIndex;
    quint32 currentStaffIndex;
    quint32 currentStaffTopEdge;
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
