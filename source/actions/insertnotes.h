#ifndef INSERTNOTES_H
#define INSERTNOTES_H

#include <QUndoCommand>
#include <vector>
#include <memory>

class Position;
class System;
class Staff;

class InsertNotes : public QUndoCommand
{
public:
    InsertNotes(std::shared_ptr<System> system, std::shared_ptr<Staff> staff,
                uint32_t insertionPos, const std::vector<Position*>& newPositions);
    ~InsertNotes();

    void redo();
    void undo();

private:
    std::shared_ptr<System> system;
    std::shared_ptr<Staff> staff;
    const uint32_t insertionPos;
    std::vector<Position*> newPositions;

    std::shared_ptr<System> originalSystem;
    bool positionsInUse;
};

#endif // INSERTNOTES_H
