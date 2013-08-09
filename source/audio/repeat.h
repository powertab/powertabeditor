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
  
#ifndef AUDIO_REPEAT_H
#define AUDIO_REPEAT_H

#include <boost/unordered_map.hpp>
#include <score/systemlocation.h>

class AlternateEnding;

/// Represents a repeat end bar, and tracks the number of remaining repeats to
/// be played, etc.
class RepeatEnd
{
public:
    RepeatEnd();
    RepeatEnd(int repeatCount);

    /// Resets the number of remaining repeats to its original value.
    void reset();

    /// Adjusts the number of remaining repeats.
    bool performRepeat();

private:
    int myRepeatCount;
    int myRemainingRepeats;
};

/// Contains all information about a repeat - start bar, end bar(s), alternate
/// endings, repeat count, etc.
class Repeat
{
public:
    Repeat(const SystemLocation &myStartBarLocation);

    /// Adds a new end bar to the repeat.
    void addRepeatEnd(const SystemLocation &location, const RepeatEnd &endBar);

    /// Adds an alternate ending to the repeat group.
    void addAlternateEnding(int system, const AlternateEnding &altEnding);

    int getActiveRepeat() const;

    /// Resets the repeat group to its original state (restores counters, etc).
    void reset();

    /// Performs a repeat event if possible.
    /// @returns The playback position to shift to.
    SystemLocation performRepeat(const SystemLocation &location);

private:
    typedef boost::unordered_map< SystemLocation, RepeatEnd,
                                  boost::hash<SystemLocation> > EndBarsMap;
    EndBarsMap myEndBars;

    typedef boost::unordered_map<int, SystemLocation> AltEndingsMap;
    AltEndingsMap myAlternateEndings;

    const SystemLocation myStartBarLocation;
    int myActiveRepeat;
};

#endif
