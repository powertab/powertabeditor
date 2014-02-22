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
  
#ifndef AUDIO_REPEATCONTROLLER_H
#define AUDIO_REPEATCONTROLLER_H

#include <map>
#include "repeat.h"
#include <score/direction.h>
#include <score/systemlocation.h>

class Score;
class System;

/// Keeps track of repeat events during playback.
class RepeatController
{
public:
    RepeatController(const Score &score);

    /// Checks if a repeat needs to be performed at the given location.
    /// @returns True if the playback position needs to be changed, and updates
    ///     the newLocation parameter with the new playback location.
    bool checkForRepeat(const SystemLocation &prevLocation,
                        const SystemLocation &currentLocation,
                        SystemLocation &newLocation);

private:
    /// Scans through the entire score and finds all pairs of repeat bars.
    void indexRepeats();
    /// Scan through all of the musical directions in the system.
    void indexDirections(int systemIndex, const System &system);

    SystemLocation performMusicalDirection(
            DirectionSymbol::SymbolType directionType);

    /// Returns the repeat section that surrounds the given position, if possible.
    Repeat *findActiveRepeat(const SystemLocation &location);

    const Score &myScore;
    /// The active musical direction symbol during playback.
    DirectionSymbol::ActiveSymbolType myActiveSymbol;
    /// Holds all repeats in the score.
    std::map<SystemLocation, Repeat> myRepeats;

    typedef std::multimap<SystemLocation, DirectionSymbol> DirectionMap;
    /// Stores each musical direction in the system.
    DirectionMap myDirections;

    typedef std::map<DirectionSymbol::SymbolType, SystemLocation> SymbolLocationsMap;
    /// Stores the location of each music symbol (coda, etc).
    SymbolLocationsMap mySymbolLocations;
};

#endif
