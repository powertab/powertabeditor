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
  
#include "midievent.h"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/version.hpp>

MidiEvent::MidiEvent(uint8_t channel, double startTime, double duration,
                     uint32_t positionIndex, uint32_t systemIndex) :
    channel(channel),
    startTime(startTime),
    duration(duration),
    positionIndex(positionIndex),
    systemIndex(systemIndex)
{
}

uint32_t MidiEvent::getPositionIndex() const
{
    return positionIndex;
}

uint32_t MidiEvent::getSystemIndex() const
{
    return systemIndex;
}

double MidiEvent::getStartTime() const
{
    return startTime;
}

double MidiEvent::getDuration() const
{
    return duration;
}

/// Compares by timestamp, then by system index, then by position index.
bool MidiEvent::operator<(const MidiEvent& event) const
{
    using namespace boost::test_tools;
    
    // compare timestamps using a floating point comparison
    if (check_is_close(startTime, event.startTime, percent_tolerance(0.001)))
    {
        if (systemIndex == event.systemIndex)
        {
            return positionIndex < event.positionIndex;
        }
        else
        {
            return systemIndex < event.systemIndex;
        }
    }
    else
    {
        return startTime < event.startTime;
    }
}
