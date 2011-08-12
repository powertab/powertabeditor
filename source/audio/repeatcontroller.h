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
  
#ifndef REPEATCONTROLLER_H
#define REPEATCONTROLLER_H

#include <cstdint>
#include <map>
#include <audio/repeat.h>
#include <audio/directionsymbol.h>
#include <memory>
#include <boost/unordered_map.hpp>

class Score;
class System;

/// Keeps track of repeat events during playback
class RepeatController
{
public:
    RepeatController(const Score* score);

    bool checkForRepeat(const SystemLocation& currentLocation, SystemLocation& newLocation);

private:
    void indexRepeats();
    void indexDirections(uint32_t systemIndex, std::shared_ptr<const System> system);

    SystemLocation performMusicalDirection(uint8_t directionType);

    Repeat& getPreviousRepeatGroup(const SystemLocation& location);

    const Score* score;
    uint8_t activeSymbol; ///< active musical direction symbol during playback

    std::map<SystemLocation, Repeat> repeats; ///< Holds all repeats in the score

    boost::unordered_multimap<SystemLocation, DirectionSymbol> directions; ///< Stores each musical direction in the system
    boost::unordered_map<uint8_t, SystemLocation> symbolLocations; ///< Stores the location of each music symbol (coda, etc)
};

#endif // REPEATCONTROLLER_H
