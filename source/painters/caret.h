#ifndef CARET_H
#define CARET_H

#include "painterbase.h"
#include "staffdata.h"

#include <memory>

class Note;
class Position;
class Staff;
class System;
class Score;
class Barline;

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

    void moveCaretToNextBar();
    void moveCaretToPrevBar();

    bool moveCaretSection(int offset);
    void moveCaretToFirstSection();
    void moveCaretToLastSection();

    bool moveCaretStaff(int offset);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void paintSelection(QPainter* painter);
    QRectF boundingRect() const;

    void setScore(Score* newScore);

    bool setCurrentStringIndex(uint8_t stringIndex);
    bool setCurrentPositionIndex(uint8_t positionIndex);
    bool setCurrentStaffIndex(uint32_t staffIndex);
    bool setCurrentSystemIndex(uint32_t systemIndex);

    Score* getCurrentScore() const;
    std::shared_ptr<System> getCurrentSystem() const;
    Staff* getCurrentStaff() const;
    Position* getCurrentPosition() const;
    Note* getCurrentNote() const;
    Barline* getCurrentBarline() const;

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

public slots:
    void updateSelection(int start, int end);

protected:
    void updateStaffInfo();

    bool inPlaybackMode;

    Score* currentScore;
    quint32 currentSystemIndex;
    quint32 currentStaffIndex;
    quint32 currentStringIndex;
    quint32 currentPositionIndex;

    quint32 currentStaffTopEdge;
    int lineSpacing;
    StaffData currentStaffInfo;

    std::pair<int, int> selectionRange;

    static const int CARET_NOTE_SPACING = 6; // spacing around a highlighted note
};

#endif // CARET_H
