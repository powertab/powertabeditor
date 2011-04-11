#ifndef POSITIONSHIFT_H
#define POSITIONSHIFT_H

#include <QUndoCommand>
#include <memory>

class System;

class PositionShift : public QUndoCommand
{
public:
    enum ShiftType
    {
        SHIFT_FORWARD,
        SHIFT_BACKWARD
    };

    PositionShift(std::shared_ptr<System> system, quint32 positionIndex, ShiftType type);
    void redo();
    void undo();

protected:
    std::shared_ptr<System> system;
    const quint32 positionIndex;
    const ShiftType type;
};

#endif // POSITIONSHIFT_H
