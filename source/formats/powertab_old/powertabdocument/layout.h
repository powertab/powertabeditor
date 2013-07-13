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
  
#ifndef LAYOUT_H
#define LAYOUT_H

#include <boost/shared_ptr.hpp>
#include <vector>

namespace PowerTabDocument {

class Score;
class System;
class Staff;

/// Contains various functions for computing the layout of staves, systems, notes, etc
namespace Layout
{
void CalculateStdNotationHeight(Score* score, boost::shared_ptr<System> system);

void CalculateTabStaffBelowSpacing(boost::shared_ptr<const System> system, boost::shared_ptr<Staff> staff);
void CalculateSymbolSpacing(const Score* score, boost::shared_ptr<System> system,
                            boost::shared_ptr<Staff> staff);

void FormatSystem(boost::shared_ptr<System> system);

void FixHammerons(boost::shared_ptr<Staff> staff);

enum SymbolType
{
    NoSymbol,
    // symbols between staves
    SymbolLetRing,
    SymbolVolumeSwell,
    SymbolVibrato,
    SymbolWideVibrato,
    SymbolPalmMuting,
    SymbolTremoloPicking,
    SymbolTremoloBar,
    SymbolTrill,
    SymbolNaturalHarmonic,
    SymbolArtificialHarmonic,
    SymbolDynamic,
    SymbolBend,
    // symbols below tab staff
    SymbolPickStrokeDown,
    SymbolPickStrokeUp,
    SymbolTap,
    SymbolHammerOnPullOff,
    SymbolSlide,
    SymbolTappedHarmonic,
    // Symbols above standard notation staff.
    SymbolOctave8va,
    SymbolOctave15ma,
    // Symbols below standard notation staff.
    SymbolOctave8vb,
    SymbolOctave15mb
};

struct SymbolGroup
{
    SymbolGroup(int voice, int leftPosIndex, int leftX, int width, int height,
                SymbolType type);

    int voice;
    int leftPosIndex; /// Position index of the start of the symbol group
    int leftX; /// x-coordinate of the start of the symbol group
    int width; /// Width (in coordinates) of the group
    int height; /// offset from the staff
    SymbolType symbolType;
};

void CalculateSymbolLayout(std::vector<Layout::SymbolGroup>& symbolGroups,
                           const Score* score,
                           boost::shared_ptr<const System> system,
                           boost::shared_ptr<const Staff> staff);

void CalculateTabStaffBelowLayout(std::vector<Layout::SymbolGroup>& symbolGroups,
                                  boost::shared_ptr<const System> system,
                                  boost::shared_ptr<const Staff> staff);

void CalculateStdNotationAboveLayout(std::vector<Layout::SymbolGroup>& symbolGroups,
                                     boost::shared_ptr<const System> system,
                                     boost::shared_ptr<const Staff> staff);

void CalculateStdNotationBelowLayout(std::vector<Layout::SymbolGroup>& symbolGroups,
                                     boost::shared_ptr<const System> system,
                                     boost::shared_ptr<const Staff> staff);
}

}

#endif // LAYOUT_H
