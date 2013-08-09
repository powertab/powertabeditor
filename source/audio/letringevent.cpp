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
  
#include "letringevent.h"

#include <audio/midioutputdevice.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

LetRingEvent::LetRingEvent(int channel, double startTime, int position,
                           int system, LetRingEvent::EventType eventType)
    : MidiEvent(channel, startTime, 0, position, system),
      myEventType(eventType)
{
}

void LetRingEvent::performEvent(MidiOutputDevice &device) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Let Ring " << ((myEventType == LET_RING_ON) ? "On" : "Off")
             << ": " << mySystem << ", " << myPosition << " at " << myStartTime;
#endif

    device.setSustain(myChannel, myEventType == LetRingOn);
}
