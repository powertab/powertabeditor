/*
  * Copyright (C) 2015 Cameron White
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
  
#ifndef MIDI_MIDIEVENT_H
#define MIDI_MIDIEVENT_H

#include <score/systemlocation.h>

#include <cstdint>
#include <vector>

class MidiEvent
{
public:
    int getTicks() const { return myTicks; }
    void setTicks(int ticks) { myTicks = ticks; }
    uint8_t getStatusByte() const { return myStatusByte; }
    const std::vector<uint8_t> &getData() const { return myData; }

    static MidiEvent endOfTrack(int ticks);
    static MidiEvent setTempo(int ticks, int microseconds);
    static MidiEvent noteOn(int ticks, uint8_t channel, uint8_t pitch,
                            uint8_t velocity, const SystemLocation &location);
    static MidiEvent noteOff(int ticks, uint8_t channel, uint8_t pitch,
                             const SystemLocation &location);
    static MidiEvent volumeChange(int ticks, uint8_t channel, uint8_t level);
    static MidiEvent programChange(int ticks, uint8_t channel, uint8_t preset);
    static MidiEvent modWheel(int ticks, uint8_t channel, uint8_t width);
    static MidiEvent holdPedal(int ticks, uint8_t channel, bool enabled);
    static MidiEvent pitchWheel(int ticks, uint8_t channel, uint8_t amount);
    static std::vector<MidiEvent> pitchWheelRange(int ticks, uint8_t channel,
                                                  uint8_t semitones);

private:
    MidiEvent(int ticks, uint8_t status, std::vector<uint8_t> data,
              const SystemLocation &location, int player, int instrument);

    int myTicks; // TODO - does this need to be 64-bit for absolute times?
    uint8_t myStatusByte;
    std::vector<uint8_t> myData;

    SystemLocation myLocation;
    int myPlayer;
    int myInstrument;
};

#endif
