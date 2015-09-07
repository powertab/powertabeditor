/*
  * Copyright (C) 2015 Cameron White
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
  
#include "midieventlist.h"

#include <algorithm>
#include <cassert>

MidiEventList::MidiEventList(bool absolute_ticks)
    : myAbsoluteTicks(absolute_ticks)
{
}

void MidiEventList::convertToDeltaTicks()
{
    assert(myAbsoluteTicks);
    myAbsoluteTicks = false;

    if (myEvents.size() <= 1)
        return;

    // First, sort by timestamp. Events for different voices may have been added
    // out of order.
    std::stable_sort(myEvents.begin(), myEvents.end(),
                     [](const MidiEvent &a, const MidiEvent &b)
                     {
                         return a.getTicks() < b.getTicks();
                     });

    for (size_t i = myEvents.size() - 1; i >= 1; --i)
    {
        MidiEvent &event = myEvents[i];
        const MidiEvent &prev_event = myEvents[i - 1];
        event.setTicks(event.getTicks() - prev_event.getTicks());
    }
}

void MidiEventList::convertToAbsoluteTicks()
{
    assert(!myAbsoluteTicks);
    myAbsoluteTicks = true;

    for (size_t i = 1; i < myEvents.size(); ++i)
    {
        myEvents[i].setTicks(myEvents[i].getTicks() +
                             myEvents[i - 1].getTicks());
    }
}

void MidiEventList::concat(const MidiEventList &other)
{
    myEvents.reserve(myEvents.size() + other.myEvents.size());
    myEvents.insert(myEvents.end(), other.myEvents.begin(),
                    other.myEvents.end());
}
