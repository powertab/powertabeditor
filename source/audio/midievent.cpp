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

#include <cmath>
#include "midioutputdevice.h"

const int MidiEvent::NUM_CHANNELS = MidiOutputDevice::NUM_CHANNELS;

MidiEvent::MidiEvent(int channel, double startTime, double duration,
                     int position, int system)
    : myChannel(channel),
      myStartTime(startTime),
      myDuration(duration),
      myPosition(position),
      mySystem(system)
{
}

int MidiEvent::getPosition() const
{
    return myPosition;
}

int MidiEvent::getSystem() const
{
    return mySystem;
}

double MidiEvent::getStartTime() const
{
    return myStartTime;
}

double MidiEvent::getDuration() const
{
    return myDuration;
}

bool MidiEvent::operator<(const MidiEvent &event) const
{
    // Compare timestamps using a floating point comparison.
    if (std::abs(myStartTime - event.myStartTime) <= 0.001 * std::abs(myStartTime))
    {
        if (mySystem == event.mySystem)
            return myPosition < event.myPosition;
        else
            return mySystem < event.mySystem;
    }
    else
        return myStartTime < event.myStartTime;
}
