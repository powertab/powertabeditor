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
  
#ifndef LETRINGEVENT_H
#define LETRINGEVENT_H

#include "midievent.h"

class LetRingEvent : public MidiEvent
{
public:
    enum EventType
    {
        LET_RING_ON,
        LET_RING_OFF
    };

    LetRingEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex,
                 EventType eventType);
    
    void performEvent(RtMidiWrapper& sequencer) const;
    
private:
    const EventType eventType;
};

#endif // LETRINGEVENT_H
