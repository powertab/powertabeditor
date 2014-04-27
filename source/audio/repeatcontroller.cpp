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

int RepeatState::getCurrentRepeatNumber() const { return myActiveRepeat; }

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

RepeatController::RepeatController(const Score &score)
    : myDirectionIndex(score),
      myRepeatIndex(score)
{
    for (const RepeatedSection &repeat : myRepeatIndex.getRepeats())
        myRepeatStates.insert(std::make_pair(&repeat, RepeatState(repeat)));
}

bool RepeatController::checkForRepeat(const SystemLocation &prevLocation,
                                      const SystemLocation &currentLocation,
                                      SystemLocation &newLocation)
{
    const RepeatedSection *activeRepeat = myRepeatIndex.findRepeat(currentLocation);
    RepeatState *repeatState = nullptr;
    if (activeRepeat)
        repeatState = &myRepeatStates.at(activeRepeat);

    newLocation = myDirectionIndex.performDirection(
        prevLocation, currentLocation,
        activeRepeat ? repeatState->getCurrentRepeatNumber() : 1);

    if (newLocation != currentLocation)
    {
        // If a direction was performed, reset the repeat count for the active
        // repeat, since we may end up returning to it later (e.g. D.C. al
        // Fine).
        if (activeRepeat)
            repeatState->reset();
    }
    // If no musical direction was performed, try to perform a repeat.
    else if (activeRepeat)
        newLocation = repeatState->performRepeat(currentLocation);

    // Return true if a position shift occurred.
    return newLocation != currentLocation;
}
