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
  
#include "repeatcontroller.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/direction.h>

#include <iostream>
#include <vector>

using boost::shared_ptr;
using std::vector;

RepeatController::RepeatController(const Score* score) :
    score(score),
    activeSymbol(Direction::activeNone)
{
    indexRepeats();
}

/// Scans through the entire score and finds all of the pairs of repeat bars
void RepeatController::indexRepeats()
{
    // add start of score as the first repeat
    const SystemLocation scoreStartLocation(0, 0);
    repeats[scoreStartLocation] = Repeat(scoreStartLocation);

    for (size_t currentSystemIndex = 0; currentSystemIndex < score->GetSystemCount(); currentSystemIndex++)
    {
        shared_ptr<const System> system = score->GetSystem(currentSystemIndex);

        vector<System::BarlineConstPtr> barlines;
        system->GetBarlines(barlines);

        for (size_t i = 0; i < barlines.size(); i++)
        {
            System::BarlineConstPtr currentBar = barlines.at(i);

            if (currentBar->IsRepeatStart())
            {
                const SystemLocation location(currentSystemIndex,
                                              currentBar->GetPosition());

                repeats[location] = Repeat(location);
            }
            else if (currentBar->IsRepeatEnd())
            {
                // add to the end bar list for the active repeat group
                repeats.rbegin()->second.addRepeatEnd(SystemLocation(currentSystemIndex, currentBar->GetPosition()),
                                                      RepeatEnd(currentBar->GetRepeatCount()));
            }
        }

        indexDirections(currentSystemIndex, system);
    }

    // add alternate endings
    for (size_t i = 0; i < score->GetAlternateEndingCount(); i++)
    {
        shared_ptr<const AlternateEnding> altEnding = score->GetAlternateEnding(i);

        const SystemLocation altEndingLocation(altEnding->GetSystem(),
                                               altEnding->GetPosition());

        Repeat& activeRepeat = getPreviousRepeatGroup(altEndingLocation);
        activeRepeat.addAlternateEnding(altEnding);
    }
}

/// Scan through all of the musical directions in the system
void RepeatController::indexDirections(uint32_t systemIndex, shared_ptr<const System> system)
{
    for (size_t i = 0; i < system->GetDirectionCount(); i++)
    {
        shared_ptr<const Direction> direction = system->GetDirection(i);

        const SystemLocation location(systemIndex, direction->GetPosition());

        for (size_t dirNum = 0; dirNum < direction->GetSymbolCount(); dirNum++)
        {
            uint32_t index = 0;
            uint8_t symbolType = 0, activeSymbol = 0, repeatNumber = 0;
            direction->GetSymbol(index, symbolType, activeSymbol, repeatNumber);

            directions.insert(std::make_pair(location,
                                             DirectionSymbol(symbolType, activeSymbol, repeatNumber)));

            symbolLocations.insert(std::make_pair(symbolType, location));
        }
    }
}

/// Returns the active repeat - the last repeat with a start bar before the given position
Repeat& RepeatController::getPreviousRepeatGroup(const SystemLocation& location)
{
    std::map<SystemLocation, Repeat>::iterator repeatGroup = repeats.upper_bound(location);
    if (repeatGroup != repeats.begin())
    {
        repeatGroup--;
    }

    return repeatGroup->second;
}

/// Checks if a repeat needs to be performed at the given system and position.
/// @return true If the playback position needs to be changed, and
/// updates the newSystem and newPos parameters with the new playback position
bool RepeatController::checkForRepeat(const SystemLocation& currentLocation,
                                      SystemLocation& newLocation)
{
    if (repeats.empty()) // no repeat events in the score
    {
        return false;
    }

    Repeat& activeRepeat = getPreviousRepeatGroup(currentLocation);
    newLocation = currentLocation;

    // check for directions at location
    std::pair<DirectionMap::iterator, DirectionMap::iterator> directionsAtLocation =
            directions.equal_range(currentLocation);

    if (directionsAtLocation.first != directions.end())
    {
        DirectionSymbol& direction = directionsAtLocation.first->second;

        if (direction.shouldPerformDirection(activeSymbol, activeRepeat.getActiveRepeat()))
        {
            newLocation = performMusicalDirection(direction.getSymbolType());

            if (newLocation != currentLocation)
            {
                // remove the direction if it was performed
                directions.erase(directionsAtLocation.first);
                // reset the repeat count for the active repeat, since we may end up returning to it later
                // (e.g. D.C. al Fine)
                activeRepeat.reset();
            }
        }
    }

    // if no musical direction was performed, try to perform a repeat
    if (newLocation == currentLocation)
    {
        newLocation = activeRepeat.performRepeat(currentLocation);
    }

    // return true if a position shift occurred
    return (newLocation != currentLocation);
}

SystemLocation RepeatController::performMusicalDirection(uint8_t directionType)
{
    if (directionType == Direction::fine) // go to the end of the score
    {
        const uint32_t lastSystemIndex = score->GetSystemCount() - 1;
        return SystemLocation(lastSystemIndex, score->GetSystem(lastSystemIndex)->GetPositionCount());
    }

    uint8_t nextSymbol = 0;

    switch(directionType)
    {
    // return to beginning of score
    case Direction::daCapo:
    case Direction::daCapoAlCoda:
    case Direction::daCapoAlDoubleCoda:
    case Direction::daCapoAlFine:
        activeSymbol = Direction::activeDaCapo;
        return SystemLocation(0, 0);
        break;

    // return to segno sign
    case Direction::dalSegno:
    case Direction::dalSegnoAlCoda:
    case Direction::dalSegnoAlDoubleCoda:
    case Direction::dalSegnoAlFine:
        activeSymbol = Direction::activeDalSegno;
        nextSymbol = Direction::segno;
        break;

    // return to segnoSegno sign
    case Direction::dalSegnoSegno:
    case Direction::dalSegnoSegnoAlCoda:
    case Direction::dalSegnoSegnoAlDoubleCoda:
    case Direction::dalSegnoSegnoAlFine:
        activeSymbol = Direction::activeDalSegnoSegno;
        nextSymbol = Direction::segnoSegno;
        break;

    // jump to coda
    case Direction::toCoda:
        nextSymbol = Direction::coda;
        break;

    // jump to double coda
    case Direction::toDoubleCoda:
        nextSymbol = Direction::doubleCoda;
        break;
    }

    // now, find the location of the symbol to jump to
    SymbolLocationsMap::iterator symbolLocation = symbolLocations.find(nextSymbol);
    if (symbolLocation != symbolLocations.end())
    {
        return symbolLocation->second;
    }
    else
    {
        // this should not happen if the score is properly written
        std::cerr << "Could not find the symbol " << static_cast<int>(nextSymbol) << std::endl;
        return SystemLocation(0, 0);
    }
}
