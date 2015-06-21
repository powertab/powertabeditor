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

#include <cstdint>
#include <vector>

enum class StatusByte : uint8_t
{
    MetaMessage = 0xff
};

enum class MetaType : uint8_t
{
    TrackEnd = 0x2f
};

class MidiEvent
{
public:
    MidiEvent(const MidiEvent &) = default;
    MidiEvent(MidiEvent &&) = default;

    int getTicks() const { return myTick; }
    StatusByte getStatusByte() const { return myStatusByte; }
    const std::vector<uint8_t> &getData() const { return myData; }

    static MidiEvent endOfTrack(int tick);

private:
    MidiEvent(int tick, StatusByte status, std::vector<uint8_t> data,
              int system, int position, int player, int instrument);

    int myTick; // TODO - does this need to be 64-bit for absolute times?
    StatusByte myStatusByte;
    std::vector<uint8_t> myData;

    int mySystem;
    int myPosition;
    int myPlayer;
    int myInstrument;
};

#endif
