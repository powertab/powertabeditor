/*
  * Copyright (C) 2014 Cameron White
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

#ifndef SCORE_UTILS_DIRECTIONINDEX_H
#define SCORE_UTILS_DIRECTIONINDEX_H

#include <map>
#include <score/direction.h>
#include <score/systemlocation.h>

class Score;

/// Indexes all of the directions in the system and provides functions for
/// determining when a particular direction should be taken.
class DirectionIndex
{
public:
    DirectionIndex(const Score &score);

    /// When moving from one location in the score to another, returns the new
    /// location that should be moved to (after taking directions into account).
    SystemLocation performDirection(const SystemLocation &prevLocation,
                                    const SystemLocation &currentLocation,
                                    int activeRepeat);

private:
    SystemLocation followDirection(DirectionSymbol::SymbolType symbol);

    const Score &myScore;
    DirectionSymbol::ActiveSymbolType myActiveSymbol;

    /// Used for looking up direction symbols at a given location.
    std::multimap<SystemLocation, DirectionSymbol> myDirections;
    /// Used for finding the location of a symbol type.
    std::map<DirectionSymbol::SymbolType, SystemLocation> mySymbolLocations;
};

#endif
