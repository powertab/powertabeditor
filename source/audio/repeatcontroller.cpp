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
    // There may be nested repeats, so maintain a stack of the active repeats
    // as we go through the score.
    std::stack<Repeat> repeats;

    // The start of the score can always act as a repeat start bar.
    repeats.push(Repeat(SystemLocation(0, 0)));

    int systemIndex = 0;
    for (const System &system : myScore.getSystems())
    {
        for (const Barline &bar : system.getBarlines())
        {
            // If we've seen the last alternate ending of the repeat,
            // we are done.
            if (!repeats.empty())
            {
                Repeat &activeRepeat = repeats.top();
                if (activeRepeat.getAlternateEndingCount() &&
                    activeRepeat.getAlternateEndingCount() ==
                        activeRepeat.getTotalRepeatCount())
                {
                    myRepeats.emplace(activeRepeat.getStartLocation(),
                                      repeats.top());
                    repeats.pop();
                }
            }

            // Record any start bars that we see.
            if (bar.getBarType() == Barline::RepeatStart)
            {
                const SystemLocation location(systemIndex, bar.getPosition());
                repeats.push(Repeat(location));
            }
            else if (bar.getBarType() == Barline::RepeatEnd)
            {
                // TODO - do a better job of handling mismatched repeats.
                Q_ASSERT(!repeats.empty());

                // Add this end bar to the active repeat.
                Repeat &activeRepeat = repeats.top();
                activeRepeat.addRepeatEnd(
                    SystemLocation(systemIndex, bar.getPosition()),
                    RepeatEnd(bar.getRepeatCount()));

                // If we don't have any alternate endings, we must be
                // done with this repeat.
                if (repeats.top().getAlternateEndingCount() == 0)
                {
                    myRepeats.emplace(repeats.top().getStartLocation(), repeats.top());
                    repeats.pop();
                }
            }

            // Process repeat endings in this bar, unless we're at the end bar.
            const Barline *nextBar = system.getNextBarline(bar.getPosition());
            if (nextBar)
            {
                for (const AlternateEnding &ending : ScoreUtils::findInRange(
                         system.getAlternateEndings(), bar.getPosition(),
                         nextBar->getPosition() - 1))
                {
                    // TODO - do a better job of handling this error.
                    Q_ASSERT(!repeats.empty());
                    repeats.top().addAlternateEnding(systemIndex, ending);
                }
            }
        }

        indexDirections(systemIndex, system);
        ++systemIndex;
    }
}

void RepeatController::indexDirections(int systemIndex, const System &system)
{
    for (const Direction &direction : system.getDirections())
    {
        const SystemLocation location(systemIndex, direction.getPosition());

        for (const DirectionSymbol &symbol : direction.getSymbols())
        {
            myDirections.insert(std::make_pair(location, symbol));
            mySymbolLocations.insert(std::make_pair(symbol.getSymbolType(),
                                                    location));
        }
    }
}

Repeat *RepeatController::findActiveRepeat(const SystemLocation &location)
{
    auto repeatGroup = myRepeats.lower_bound(location);

    // Search for a pair of start and end bars that surrounds this location.
    while (repeatGroup != myRepeats.begin())
    {
        --repeatGroup;
        if (repeatGroup->second.getLastEndBarLocation() >= location)
            return &repeatGroup->second;
    }

    return nullptr;
}

bool RepeatController::checkForRepeat(const SystemLocation &prevLocation,
                                      const SystemLocation &currentLocation,
                                      SystemLocation &newLocation)
{
    // No repeat events in the score.
    if (myRepeats.empty())
        return false;

    Repeat *activeRepeat = findActiveRepeat(currentLocation);
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
                activeRepeat ? activeRepeat->getActiveRepeatNumber() : 1))
        {
            newLocation = performMusicalDirection(direction.getSymbolType());

            if (newLocation != currentLocation)
            {
                // Remove the direction if it was performed.
                myDirections.erase(leftIt);
                // Reset the repeat count for the active repeat, since we may
                // end up returning to it later (e.g. D.C. al Fine).
                if (activeRepeat)
                    activeRepeat->reset();
            }
        }
    }

    // If no musical direction was performed, try to perform a repeat.
    if (newLocation == currentLocation && activeRepeat)
        newLocation = activeRepeat->performRepeat(currentLocation);

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
