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

    bool setCurrentStringIndex(uint8_t stringIndex);
    bool setCurrentPositionIndex(uint8_t positionIndex);
    bool setCurrentStaffIndex(uint32_t staffIndex);
    bool setCurrentSystemIndex(uint32_t systemIndex);

    Score* getCurrentScore() const;
    System* getCurrentSystem() const;
    Staff* getCurrentStaff() const;
    Position* getCurrentPosition() const;
    Note* getCurrentNote() const;

    inline quint32 getCurrentSystemIndex() const
    {
        return currentSystemIndex;
    }

    inline quint32 getCurrentStaffIndex() const
    {
        return currentStaffIndex;
    }

    inline quint32 getCurrentPositionIndex() const
    {
        return currentPositionIndex;
    }

    inline quint32 getCurrentStringIndex() const
    {
        return currentStringIndex;
    }

signals:
    void moved();

protected:
    void updateStaffInfo();
    void moveToNewPosition();

    bool inPlaybackMode;

    Score* currentScore;
    quint32 currentSystemIndex;
    quint32 currentStaffIndex;
    quint32 currentStringIndex;
    quint32 currentPositionIndex;

    quint32 currentStaffTopEdge;
    int lineSpacing;
    StaffData currentStaffInfo;

    static const int CARET_NOTE_SPACING = 6; // spacing around a highlighted note
};

#endif // CARET_H
