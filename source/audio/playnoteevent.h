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
  
#ifndef PLAYNOTEEVENT_H
#define PLAYNOTEEVENT_H

#include "midievent.h"

#include <memory>

class Guitar;

class PlayNoteEvent : public MidiEvent
{
public:
    enum VelocityType
    {
        DEFAULT_VELOCITY = 127,
        MUTED_VELOCITY = 60,
        GHOST_VELOCITY = 50,
        PALM_MUTED_VELOCITY = 112
    };

    PlayNoteEvent(uint8_t channel, double startTime, double duration, uint8_t pitch, uint32_t positionIndex,
                  uint32_t systemIndex, std::shared_ptr<const Guitar> guitar, bool isMuted, VelocityType velocity);

    void performEvent(RtMidiWrapper& sequencer) const;

protected:
    uint8_t pitch;
    std::shared_ptr<const Guitar> guitar;
    bool isMuted;
    VelocityType velocity;
};

#endif // PLAYNOTEEVENT_H
