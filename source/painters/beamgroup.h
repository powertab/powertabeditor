/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
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

    static const double FRACTIONAL_BEAM_WIDTH;

    void copyNoteSteams(std::vector<NoteStem>& stems) const;

private:
    void setStemDirections();
    void adjustStemHeights();

    StaffData staffInfo;
    std::vector<NoteStem> noteStems;
    NoteStem::StemDirection stemDirection;
};

#endif // BEAMGROUP_H
