#ifndef NOTESTEM_H
#define NOTESTEM_H

#include <vector>
#include "staffdata.h"

class Position;
class StaffData;
class QGraphicsItem;

class NoteStem
{
public:
    enum StemDirection
    {
        StemUp,
        StemDown
    };

    NoteStem(const StaffData& staffInfo, const Position* position,
             double x, const std::vector<double>& noteLocations);

    const Position* position() const;

    double x() const;
    void setX(double x);

    double top() const;
    void setTop(double top);

    double bottom() const;
    void setBottom(double bottom);

    StemDirection direction() const;
    void setDirection(StemDirection direction);

    double stemSize() const;

    QGraphicsItem* createNoteFlag() const;
    QGraphicsItem* createStaccato() const;
    QGraphicsItem* createFermata() const;
    QGraphicsItem* createAccent() const;

    double stemEdge() const;

private:
    const Position* position_;
    double x_;
    double top_;
    double bottom_;
    double stemSize_;
    StemDirection direction_;
    StaffData staffInfo_;
};

#endif // NOTESTEM_H
