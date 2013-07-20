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
  
#ifndef VIBRATOEVENT_H
#define VIBRATOEVENT_H

#include "midievent.h"

class VibratoEvent : public MidiEvent
{
public:
    enum VibratoType
    {
        NORMAL_VIBRATO,
        WIDE_VIBRATO
    };

    enum EventType
    {
        VIBRATO_ON,
        VIBRATO_OFF
    };

    VibratoEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex,
                 EventType eventType, VibratoType vibratoType = NORMAL_VIBRATO);

    void performEvent(RtMidiWrapper& sequencer) const;

private:
    EventType eventType;
    VibratoType vibratoType;
};

#endif // VIBRATOEVENT_H
