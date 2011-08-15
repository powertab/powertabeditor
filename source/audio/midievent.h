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
  
#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <boost/cstdint.hpp>

class RtMidiWrapper;

class MidiEvent
{
public:
    static const uint8_t NUM_CHANNELS = 16;

    MidiEvent(uint8_t channel, double startTime, double duration, uint32_t positionIndex, uint32_t systemIndex);
    virtual ~MidiEvent() {}

    bool operator<(const MidiEvent& event) const;

    virtual void performEvent(RtMidiWrapper& sequencer) const = 0;

    uint32_t getPositionIndex() const;
    uint32_t getSystemIndex() const;
    double getDuration() const;
    double getStartTime() const;

protected:
    uint8_t channel;
    double startTime;
    double duration;
    uint32_t positionIndex; ///< position in the staff that the event occurred at
    uint32_t systemIndex; ///< system that the event occurred in
};

#endif // MIDIEVENT_H
