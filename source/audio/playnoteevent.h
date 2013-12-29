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
  
#ifndef AUDIO_PLAYNOTEEVENT_H
#define AUDIO_PLAYNOTEEVENT_H

#include <cstdint>
#include "midievent.h"

class Instrument;
class Player;

class PlayNoteEvent : public MidiEvent
{
public:
    enum VelocityType
    {
        DefaultVelocity = 127,
        MutedVelocity = 60,
        GhostVelocity = 50,
        PalmMutedVelocity = 112
    };

    PlayNoteEvent(int channel, double startTime, double duration, uint8_t pitch,
                  int position, int systemIndex, const Player &player,
                  const Instrument &instrument, bool isMuted,
                  VelocityType velocity);

    virtual void performEvent(MidiOutputDevice &device) const override;

private:
    const uint8_t myPitch;
    const Player &myPlayer;
    const Instrument &myInstrument;
    const bool myIsMuted;
    const VelocityType myVelocity;
};

#endif
