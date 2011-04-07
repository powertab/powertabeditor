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

    void copyNoteSteams(std::vector<NoteStem>& stems) const;

private:
    void setStemDirections();
    void adjustStemHeights();

    StaffData staffInfo;
    std::vector<NoteStem> noteStems;
    NoteStem::StemDirection stemDirection;
};

#endif // BEAMGROUP_H
