#ifndef UPDATETABNUMBER_H
#define UPDATETABNUMBER_H

#include <QUndoCommand>

#include <powertabdocument/staff.h>

class Staff;
class Position;
class Note;

class UpdateTabNumber : public QUndoCommand
{
public:
    UpdateTabNumber(uint8_t typedNumber, Note* note, Position* position, Staff* staff);
    ~UpdateTabNumber();
    void undo();
    void redo();

protected:
    Note* note;
    Position* position;
    Staff* staff;
    uint8_t newFretNumber;
    uint8_t prevFretNumber;
    Note *origPrevNote;
    Note *origNextNote;
    Note *origNote;
};

#endif // UPDATETABNUMBER_H
