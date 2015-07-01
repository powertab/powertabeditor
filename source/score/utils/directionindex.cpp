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

#include "directionindex.h"

#include <iostream>
#include <score/score.h>

DirectionIndex::DirectionIndex(const Score &score)
    : myScore(score), myActiveSymbol(DirectionSymbol::ActiveNone)
{
    int i = 0;
    for (const System &system : score.getSystems())
    {
        for (const Direction &direction : system.getDirections())
        {
            const SystemLocation location(i, direction.getPosition());

            for (const DirectionSymbol &symbol : direction.getSymbols())
            {
                myDirections.insert(std::make_pair(location, symbol));
                mySymbolLocations.insert(
                    std::make_pair(symbol.getSymbolType(), location));
            }
        }

        ++i;
    }
}

/// Determines whether a direction should be performed, based on the
/// active symbol and repeat number.
/// If the direction's activation symbol is None, it will always be able to
/// activate regardless of the currently active symbol.
static bool shouldPerformDirection(
    const DirectionSymbol &symbol,
    DirectionSymbol::ActiveSymbolType activeSymbol, int activeRepeat)
{
    return (symbol.getActiveSymbolType() == DirectionSymbol::ActiveNone ||
            symbol.getActiveSymbolType() == activeSymbol) &&
           symbol.getSymbolType() >= DirectionSymbol::Fine &&
           (symbol.getRepeatNumber() == 0 ||
            symbol.getRepeatNumber() == activeRepeat);
}

SystemLocation DirectionIndex::performDirection(
    const SystemLocation &prevLocation, const SystemLocation &currentLocation,
    int activeRepeat)
{
    SystemLocation newLocation = currentLocation;

    // Check for directions between the previous playback location and the
    // current location.
    auto leftIt = myDirections.lower_bound(prevLocation);
    auto rightIt = myDirections.upper_bound(currentLocation);

    if (leftIt != myDirections.end() && leftIt != rightIt)
    {
        DirectionSymbol &direction = leftIt->second;

        if (shouldPerformDirection(direction, myActiveSymbol, activeRepeat))
        {
            newLocation = followDirection(direction.getSymbolType());
            myDirections.erase(leftIt);
        }
    }

    return newLocation;
}

SystemLocation DirectionIndex::followDirection(DirectionSymbol::SymbolType type)
{
    // Go to the end of the score.
    if (type == DirectionSymbol::Fine)
    {
        const auto lastSystemIndex = myScore.getSystems().size() - 1;
        return SystemLocation(static_cast<int>(lastSystemIndex),
                              myScore.getSystems()[lastSystemIndex]
                                  .getBarlines()
                                  .back()
                                  .getPosition());
    }

    DirectionSymbol::SymbolType nextSymbol = DirectionSymbol::Coda;

    switch (type)
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
    auto symbolLocation = mySymbolLocations.find(nextSymbol);
    if (symbolLocation != mySymbolLocations.end())
        return symbolLocation->second;
    else
    {
        // This should not happen if the score is properly written.
        std::cerr << "Could not find the symbol "
                  << static_cast<int>(nextSymbol) << std::endl;
        return SystemLocation(0, 0);
    }
}
