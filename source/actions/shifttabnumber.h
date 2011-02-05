#ifndef SHIFTTABNUMBER_H
#define SHIFTTABNUMBER_H

#include <QUndoCommand>

#include <powertabdocument/position.h>
class Note;
class Tuning;

class ShiftTabNumber : public QUndoCommand
{
public:
    ShiftTabNumber(Position* currentPos, Note* note, Position::ShiftType direction, quint8 numStringsInStaff, const Tuning& tuning);
    void undo();
    void redo();

protected:
    Position* currentPos;
    Note* note;
    Position::ShiftType direction;
    quint8 numStringsInStaff;
    const Tuning& tuning;
};

#endif // SHIFTTABNUMBER_H
