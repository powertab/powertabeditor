#ifndef CARET_H
#define CARET_H

#include "painterbase.h"
#include "staffdata.h"

#include <memory>
#include <cstdint>

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
    std::shared_ptr<Staff> getCurrentStaff() const;
    Position* getCurrentPosition() const;
    Note* getCurrentNote() const;
    Barline* getCurrentBarline() const;

    uint32_t getCurrentVoice() const;
    bool setCurrentVoice(uint32_t voice);

    inline uint32_t getCurrentSystemIndex() const
    {
        return currentSystemIndex;
    }

    inline uint32_t getCurrentStaffIndex() const
    {
        return currentStaffIndex;
    }

    inline uint32_t getCurrentPositionIndex() const
    {
        return currentPositionIndex;
    }

    inline uint32_t getCurrentStringIndex() const
    {
        return currentStringIndex;
    }

    std::vector<Position*> getSelectedPositions() const;
    std::vector<Note*> getSelectedNotes() const;

signals:
    void moved();

public slots:
    void updateSelection(int start, int end);

protected:
    void updateStaffInfo();

    bool inPlaybackMode;

    Score* currentScore;
    uint32_t currentSystemIndex;
    uint32_t currentStaffIndex;
    uint32_t currentStringIndex;
    uint32_t currentPositionIndex;

    uint32_t currentVoice;

    uint32_t currentStaffTopEdge;
    int lineSpacing;
    StaffData currentStaffInfo;

    std::pair<int, int> selectionRange;

    static const int CARET_NOTE_SPACING = 6; ///< spacing around a highlighted note
};

#endif // CARET_H
