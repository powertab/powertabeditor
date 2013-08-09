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
  
#include "repeat.h"

#include <boost/foreach.hpp>
#include <score/alternateending.h>

Repeat::Repeat(const SystemLocation &startBarLocation)
    : myStartBarLocation(startBarLocation),
      myActiveRepeat(1)
{
}

void Repeat::addRepeatEnd(const SystemLocation &location,
                          const RepeatEnd &endBar)
{
    myEndBars[location] = endBar;
}

void Repeat::addAlternateEnding(int system, const AlternateEnding &altEnding)
{
    const SystemLocation location(system, altEnding.getPosition());
    const std::vector<int> numbers = altEnding.getNumbers();

    // For each repeat that the ending is active, add it to the alternate
    // endings map along with the location of the ending.
    BOOST_FOREACH(int num, numbers)
    {
        myAlternateEndings[num] = location;
    }
}

SystemLocation Repeat::performRepeat(const SystemLocation &currentLocation)
{
    // Deal with alternate endings - if we are at the start of the first
    // alternate ending, we can branch off to other alternate endings depending
    // on the active repeat.
    AltEndingsMap::const_iterator firstAltEnding = myAlternateEndings.find(1);
    if (firstAltEnding != myAlternateEndings.end() &&
        firstAltEnding->second == currentLocation)
    {
        // Branch off to the next alternate ending, if it exists.
        AltEndingsMap::const_iterator nextAltEnding = myAlternateEndings.find(
                    myActiveRepeat);
        if (nextAltEnding != myAlternateEndings.end())
        {
            return nextAltEnding->second;
        }
    }

    // Now, we can look for repeat end bars.
    EndBarsMap::iterator repeatEnd = myEndBars.find(currentLocation);

    // No repeat bar.
    if (repeatEnd == myEndBars.end())
        return currentLocation;
    // Repeat event was performed.
    else if (repeatEnd->second.performRepeat())
    {
        myActiveRepeat++;
        return myStartBarLocation;
    }
    // Repeat end bar exists, but no repeat event was performed.
    else
        return currentLocation;
}

int Repeat::getActiveRepeat() const
{
    return myActiveRepeat;
}

void Repeat::reset()
{
    myActiveRepeat = 1;

    for (EndBarsMap::iterator i = myEndBars.begin(); i != myEndBars.end(); ++i)
    {
        i->second.reset();
    }
}

RepeatEnd::RepeatEnd()
    : myRepeatCount(0),
      myRemainingRepeats(0)
{
}

RepeatEnd::RepeatEnd(int repeatCount)
    : myRepeatCount(repeatCount),
      myRemainingRepeats(repeatCount - 1)
{
}

bool RepeatEnd::performRepeat()
{
    if (myRemainingRepeats != 0)
    {
        myRemainingRepeats--;
        return true;
    }
    else
    {
        myRemainingRepeats = myRepeatCount - 1;
        return false;
    }
}

void RepeatEnd::reset()
{
    myRemainingRepeats = myRepeatCount - 1;
}
