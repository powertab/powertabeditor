#ifndef SHIFTTABNUMBER_H
#define SHIFTTABNUMBER_H

#include <QUndoCommand>

#include <powertabdocument/position.h>

class Note;
class Tuning;
class Caret;

class ShiftTabNumber : public QUndoCommand
{
public:
    ShiftTabNumber(Caret* caret, Position* currentPos, Note* note, Position::ShiftType direction, quint8 numStringsInStaff, const Tuning& tuning);
    void undo();
    void redo();

protected:
    Caret* caret;
    Position* currentPos;
    Note* note;
    Position::ShiftType direction;
    quint8 numStringsInStaff;
    const Tuning& tuning;
};

#endif // SHIFTTABNUMBER_H
