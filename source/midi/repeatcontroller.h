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
  
#ifndef MIDI_REPEATCONTROLLER_H
#define MIDI_REPEATCONTROLLER_H

#include <score/utils/directionindex.h>
#include <score/utils/repeatindexer.h>

class Score;
class SystemLocation;

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
    DirectionIndex myDirectionIndex;
    RepeatIndexer myRepeatIndex;
};

#endif
