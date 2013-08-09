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

#include <boost/foreach.hpp>
#include <score/score.h>

/// Determines whether a direction should be performed, based on the
/// active symbol and repeat number.
/// If the direction's activation symbol is None, it will always be able to
/// activate regardless of the currently active symbol.
static bool shouldPerformDirection(const DirectionSymbol &symbol,
        DirectionSymbol::ActiveSymbolType activeSymbol, int activeRepeat)
{
    return (symbol.getActiveSymbolType() == DirectionSymbol::ActiveNone ||
            symbol.getActiveSymbolType() == activeSymbol) &&
            symbol.getSymbolType() >= DirectionSymbol::Fine &&
            (symbol.getRepeatNumber() == 0 ||
             symbol.getRepeatNumber() == activeRepeat);
}

RepeatController::RepeatController(const Score &score)
    : myScore(score),
      myActiveSymbol(DirectionSymbol::ActiveNone)
{
    indexRepeats();
}

void RepeatController::indexRepeats()
{
    // Add start of score as the first repeat.
    const SystemLocation scoreStart(0, 0);
    myRepeats.insert(std::make_pair(scoreStart, Repeat(scoreStart)));

    int systemIndex = 0;
    BOOST_FOREACH(const System &system, myScore.getSystems())
    {
        BOOST_FOREACH(const Barline &bar, system.getBarlines())
        {
            if (bar.getBarType() == Barline::RepeatStart)
            {
                const SystemLocation location(systemIndex, bar.getPosition());
                myRepeats.insert(std::make_pair(location, Repeat(location)));
            }
            else if (bar.getBarType() == Barline::RepeatEnd)
            {
                // Add to the end bar list for the active repeat group.
                myRepeats.rbegin()->second.addRepeatEnd(
                            SystemLocation(systemIndex, bar.getPosition()),
                            RepeatEnd(bar.getRepeatCount()));
            }
        }

        indexDirections(systemIndex, system);
        ++systemIndex;
    }

    // Add alternate endings.
    systemIndex = 0;
    BOOST_FOREACH(const System &system, myScore.getSystems())
    {
        BOOST_FOREACH(const AlternateEnding &ending,
                      system.getAlternateEndings())
        {
            const SystemLocation altEndingLocation(systemIndex,
                                                   ending.getPosition());

            Repeat& activeRepeat = getPreviousRepeatGroup(altEndingLocation);
            activeRepeat.addAlternateEnding(systemIndex, ending);
        }

        ++systemIndex;
    }
}

void RepeatController::indexDirections(int systemIndex, const System &system)
{
    BOOST_FOREACH(const Direction &direction, system.getDirections())
    {
        const SystemLocation location(systemIndex, direction.getPosition());

        BOOST_FOREACH(const DirectionSymbol &symbol, direction.getSymbols())
        {
            myDirections.insert(std::make_pair(location, symbol));
            mySymbolLocations.insert(std::make_pair(symbol.getSymbolType(),
                                                    location));
        }
    }
}

Repeat &RepeatController::getPreviousRepeatGroup(const SystemLocation &location)
{
    std::map<SystemLocation, Repeat>::iterator repeatGroup =
            myRepeats.upper_bound(location);
    if (repeatGroup != myRepeats.begin())
    {
        --repeatGroup;
    }

    return repeatGroup->second;
}

bool RepeatController::checkForRepeat(const SystemLocation &prevLocation,
                                      const SystemLocation &currentLocation,
                                      SystemLocation &newLocation)
{
    // No repeat events in the score.
    if (myRepeats.empty())
        return false;

    Repeat &activeRepeat = getPreviousRepeatGroup(currentLocation);
    newLocation = currentLocation;

    // Check for directions between the previous playback location and the
    // current location.
    DirectionMap::iterator leftIt = myDirections.lower_bound(prevLocation);
    DirectionMap::iterator rightIt = myDirections.upper_bound(currentLocation);

    if (leftIt != myDirections.end() && leftIt != rightIt)
    {
        DirectionSymbol &direction = leftIt->second;

        if (shouldPerformDirection(direction, myActiveSymbol,
                                   activeRepeat.getActiveRepeat()))
        {
            newLocation = performMusicalDirection(direction.getSymbolType());

            if (newLocation != currentLocation)
            {
                // Remove the direction if it was performed.
                myDirections.erase(leftIt);
                // Reset the repeat count for the active repeat, since we may
                // end up returning to it later (e.g. D.C. al Fine).
                activeRepeat.reset();
            }
        }
    }

    // If no musical direction was performed, try to perform a repeat.
    if (newLocation == currentLocation)
    {
        newLocation = activeRepeat.performRepeat(currentLocation);
    }

    // Return true if a position shift occurred.
    return newLocation != currentLocation;
}

SystemLocation RepeatController::performMusicalDirection(
        DirectionSymbol::SymbolType directionType)
{
    // Go to the end of the score.
    if (directionType == DirectionSymbol::Fine)
    {
        const int lastSystemIndex = myScore.getSystems().size() - 1;
        return SystemLocation(lastSystemIndex,
                    myScore.getSystems()[lastSystemIndex].getBarlines()
                              .back().getPosition());
    }

    DirectionSymbol::SymbolType nextSymbol = DirectionSymbol::Coda;

    switch (directionType)
    {
    // Return to beginning of score.
    case DirectionSymbol::DaCapo:
    case DirectionSymbol::DaCapoAlCoda:
    case DirectionSymbol::DaCapoAlDoubleCoda:
    case DirectionSymbol::DaCapoAlFine:
        myActiveSymbol = DirectionSymbol::ActiveDaCapo;
        return SystemLocation(0, 0);
        break;

    // Return to Segno sign.
    case DirectionSymbol::DalSegno:
    case DirectionSymbol::DalSegnoAlCoda:
    case DirectionSymbol::DalSegnoAlDoubleCoda:
    case DirectionSymbol::DalSegnoAlFine:
        myActiveSymbol = DirectionSymbol::ActiveDalSegno;
        nextSymbol = DirectionSymbol::Segno;
        break;

    // Return to SegnoSegno sign.
    case DirectionSymbol::DalSegnoSegno:
    case DirectionSymbol::DalSegnoSegnoAlCoda:
    case DirectionSymbol::DalSegnoSegnoAlDoubleCoda:
    case DirectionSymbol::DalSegnoSegnoAlFine:
        myActiveSymbol = DirectionSymbol::ActiveDalSegnoSegno;
        nextSymbol = DirectionSymbol::SegnoSegno;
        break;

    // Jump to coda.
    case DirectionSymbol::ToCoda:
        nextSymbol = DirectionSymbol::Coda;
        break;

    // Jump to double coda.
    case DirectionSymbol::ToDoubleCoda:
        nextSymbol = DirectionSymbol::DoubleCoda;
        break;

    default:
        break;
    }

    // Now, find the location of the symbol to jump to.
    SymbolLocationsMap::iterator symbolLocation = mySymbolLocations.find(
                nextSymbol);
    if (symbolLocation != mySymbolLocations.end())
    {
        return symbolLocation->second;
    }
    else
    {
        // This should not happen if the score is properly written.
        std::cerr << "Could not find the symbol " <<
                     static_cast<int>(nextSymbol) << std::endl;
        return SystemLocation(0, 0);
    }
}
