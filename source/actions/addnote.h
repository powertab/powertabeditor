#ifndef ADDNOTE_H
#define ADDNOTE_H

#include <QUndoCommand>
#include <cstdint>
#include <memory>

class Staff;
class Position;

class AddNote : public QUndoCommand
{
public:
    AddNote(uint8_t stringNum, uint8_t fretNumber, uint32_t positionIndex,
            uint32_t voice, std::shared_ptr<Staff> staff);
    ~AddNote();

    void redo();
    void undo();

private:
    const uint8_t stringNum;
    const uint8_t fretNumber;
    uint32_t positionIndex;
    uint32_t voice;
    std::shared_ptr<Staff> staff;

    bool newPositionAdded;
    Position* position;
};

#endif // ADDNOTE_H
