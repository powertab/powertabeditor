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
#include <score/direction.h>
#include <score/systemlocation.h>
#include <score/utils/repeatindexer.h>

class Score;
class System;

class RepeatState
{
public:
    RepeatState(const RepeatedSection &section);

    int getCurrentRepeatNumber() const;
    void reset();
    SystemLocation performRepeat(const SystemLocation &loc);

private:
    const RepeatedSection &myRepeatedSection;
    std::unordered_map<SystemLocation, int> myRemainingRepeats;
    int myActiveRepeat;
};

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
    /// Scan through all of the musical directions in the system.
    void indexDirections();

    SystemLocation performMusicalDirection(
            DirectionSymbol::SymbolType directionType);

    RepeatIndexer myIndex;
    const Score &myScore;

    /// The active musical direction symbol during playback.
    DirectionSymbol::ActiveSymbolType myActiveSymbol;

    std::unordered_map<const RepeatedSection *, RepeatState> myRepeatStates;

    typedef std::multimap<SystemLocation, DirectionSymbol> DirectionMap;
    /// Stores each musical direction in the system.
    DirectionMap myDirections;

    typedef std::map<DirectionSymbol::SymbolType, SystemLocation> SymbolLocationsMap;
    /// Stores the location of each music symbol (coda, etc).
    SymbolLocationsMap mySymbolLocations;
};

#endif
