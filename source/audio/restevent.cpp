/*
  * Copyright (C) 2012 Cameron White
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

#include "restevent.h"

RestEvent::RestEvent(uint8_t channel, double startTime, double duration,
                     uint32_t positionIndex, uint32_t systemIndex) :
    MidiEvent(channel, startTime, duration, positionIndex, systemIndex)
{
}

void RestEvent::performEvent(RtMidiWrapper &) const
{
    // Do nothing.
}
