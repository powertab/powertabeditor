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

RepeatController::RepeatController(const Score &score)
    : myDirectionIndex(score),
      myRepeatIndex(score)
{
}

bool RepeatController::checkForRepeat(const SystemLocation &prevLocation,
                                      const SystemLocation &currentLocation,
                                      SystemLocation &newLocation)
{
    RepeatedSection *active_repeat = myRepeatIndex.findRepeat(currentLocation);

    newLocation = myDirectionIndex.performDirection(
        prevLocation, currentLocation,
        active_repeat ? active_repeat->getCurrentRepeatNumber() : 1);

    if (newLocation != currentLocation)
    {
        // If a direction was performed, reset the repeat count for the active
        // repeat, since we may end up returning to it later (e.g. D.C. al
        // Fine).
        if (active_repeat)
            active_repeat->reset();
    }
    // If no musical direction was performed, try to perform a repeat.
    else if (active_repeat)
        newLocation = active_repeat->performRepeat(currentLocation);

    // Return true if a position shift occurred.
    return newLocation != currentLocation;
}
