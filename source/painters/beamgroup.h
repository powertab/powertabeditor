#ifndef BEAMGROUP_H
#define BEAMGROUP_H

#include <vector>
#include "notestem.h"
#include "staffdata.h"

class QGraphicsItem;

class BeamGroup
{
public:
    BeamGroup(const StaffData& staffInfo, const std::vector<NoteStem>& noteStems);

    void drawStems(QGraphicsItem* parent) const;
    void drawExtraBeams(QGraphicsItem* parent) const;

    static const double FRACTIONAL_BEAM_WIDTH = 5;

private:
    void setStemDirections();
    void adjustStemHeights();

    StaffData staffInfo_;
    std::vector<NoteStem> noteStems_;
    NoteStem::StemDirection stemDirection_;
};

#endif // BEAMGROUP_H
