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
  
#ifndef BENDEVENT_H
#define BENDEVENT_H

#include "midievent.h"

class Note;

class BendEvent : public MidiEvent
{
public:
    BendEvent(uint8_t channel, double startTime, uint32_t positionIndex,
              uint32_t systemIndex, uint8_t bendAmount);

    void performEvent(RtMidiWrapper& sequencer) const;

    static const uint8_t PITCH_BEND_RANGE;
    static const uint8_t DEFAULT_BEND;
    static const double BEND_QUARTER_TONE;

private:
    const uint8_t bendAmount;
};

#endif // BENDEVENT_H
