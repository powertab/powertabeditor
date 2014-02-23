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

#include <iostream>
#include <score/score.h>
#include <score/utils.h>
#include <stack>

RepeatState::RepeatState(const RepeatedSection &repeat)
    : myRepeatedSection(repeat), myActiveRepeat(1)
{
    reset();
}

int RepeatState::getCurrentRepeatNumber() const
{
    return myActiveRepeat;
}

void RepeatState::reset()
{
    myActiveRepeat = 1;

    // Reset the number of remaining repeats to the original values.
    for (auto &repeat : myRepeatedSection.getRepeatEndBars())
        myRemainingRepeats[repeat.first] = repeat.second - 1;
}

SystemLocation RepeatState::performRepeat(const SystemLocation &loc)
{
    // Deal with alternate endings - if we are at the start of the first
    // alternate ending, we can branch off to other alternate endings depending
    // on the active repeat.
    boost::optional<SystemLocation> firstAltEnding =
        myRepeatedSection.findAlternateEnding(1);
    if (firstAltEnding && *firstAltEnding == loc)
    {
        // Branch off to the next alternate ending, if it exists.
        boost::optional<SystemLocation> nextAltEnding =
            myRepeatedSection.findAlternateEnding(myActiveRepeat);
        if (nextAltEnding)
            return *nextAltEnding;
    }

    // Now, we can look for repeat end bars.
    auto remainingRepeatCount = myRemainingRepeats.find(loc);

    // No repeat bar.
    if (remainingRepeatCount == myRemainingRepeats.end())
        return loc;
    // Perform the repeat event.
    else if (remainingRepeatCount->second != 0)
    {
        --remainingRepeatCount->second;
        ++myActiveRepeat;
        return myRepeatedSection.getStartBarLocation();
    }
    // Otherwise, the repeat is not performed and is reset.
    else
    {
        remainingRepeatCount->second = myRepeatedSection.getRepeatEndBars().at(
            remainingRepeatCount->first);
        return loc;
    }
}

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
    : myIndex(score),
      myScore(score),
      myActiveSymbol(DirectionSymbol::ActiveNone)
{
    for (const RepeatedSection &repeat : myIndex.getRepeats())
        myRepeatStates.emplace(&repeat, RepeatState(repeat));

    indexDirections();
}

void RepeatController::indexDirections()
{
    int systemIndex = 0;
    for (const System &system : myScore.getSystems())
    {
        for (const Direction &direction : system.getDirections())
        {
            const SystemLocation location(systemIndex, direction.getPosition());

            for (const DirectionSymbol &symbol : direction.getSymbols())
            {
                myDirections.insert(std::make_pair(location, symbol));
                mySymbolLocations.insert(
                    std::make_pair(symbol.getSymbolType(), location));
            }
        }
        ++systemIndex;
    }
}

bool RepeatController::checkForRepeat(const SystemLocation &prevLocation,
                                      const SystemLocation &currentLocation,
                                      SystemLocation &newLocation)
{
    const RepeatedSection *activeRepeat = myIndex.findRepeat(currentLocation);
    RepeatState *repeatState = nullptr;
    if (activeRepeat)
        repeatState = &myRepeatStates.at(activeRepeat);

    newLocation = currentLocation;

    // Check for directions between the previous playback location and the
    // current location.
    auto leftIt = myDirections.lower_bound(prevLocation);
    auto rightIt = myDirections.upper_bound(currentLocation);

    if (leftIt != myDirections.end() && leftIt != rightIt)
    {
        DirectionSymbol &direction = leftIt->second;

        if (shouldPerformDirection(
                direction, myActiveSymbol,
                activeRepeat ? repeatState->getCurrentRepeatNumber() : 1))
        {
            newLocation = performMusicalDirection(direction.getSymbolType());

            if (newLocation != currentLocation)
            {
                // Remove the direction if it was performed.
                myDirections.erase(leftIt);
                // Reset the repeat count for the active repeat, since we may
                // end up returning to it later (e.g. D.C. al Fine).
                if (activeRepeat)
                    repeatState->reset();
            }
        }
    }

    // If no musical direction was performed, try to perform a repeat.
    if (newLocation == currentLocation && activeRepeat)
        newLocation = repeatState->performRepeat(currentLocation);

    // Return true if a position shift occurred.
    return newLocation != currentLocation;
}

SystemLocation RepeatController::performMusicalDirection(
        DirectionSymbol::SymbolType directionType)
{
    // Go to the end of the score.
    if (directionType == DirectionSymbol::Fine)
    {
        const auto lastSystemIndex = myScore.getSystems().size() - 1;
        return SystemLocation(static_cast<int>(lastSystemIndex),
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
