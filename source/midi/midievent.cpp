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
  
#include "midievent.h"

enum StatusByte : uint8_t
{
    NoteOff = 0x80,
    NoteOn = 0x90,
    ControlChange = 0xb0,
    ProgramChange = 0xc0,
    MetaMessage = 0xff
};

enum Controller : uint8_t
{
    ChannelVolume = 0x07
};

enum MetaType : uint8_t
{
    TrackEnd = 0x2f,
    SetTempo = 0x51
};

MidiEvent::MidiEvent(int ticks, uint8_t status, std::vector<uint8_t> data,
                     const SystemLocation &location, int player, int instrument)
    : myTicks(ticks),
      myStatusByte(status),
      myData(std::move(data)),
      myLocation(location),
      myPlayer(player),
      myInstrument(instrument)
{
}

MidiEvent MidiEvent::endOfTrack(int ticks)
{
    return MidiEvent(ticks, StatusByte::MetaMessage, { MetaType::TrackEnd, 0 },
                     SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::setTempo(int ticks, int microseconds)
{
    const uint32_t val = microseconds;
    return MidiEvent(ticks, StatusByte::MetaMessage,
                     { static_cast<uint8_t>(MetaType::SetTempo), 3,
                       static_cast<uint8_t>((val >> 16) & 0xff),
                       static_cast<uint8_t>((val >> 8) & 0xff),
                       static_cast<uint8_t>(val & 0xff) },
                     SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::noteOn(int ticks, uint8_t channel, uint8_t pitch,
                            uint8_t velocity, const SystemLocation &location)
{
    return MidiEvent(ticks, StatusByte::NoteOn + channel, { pitch, velocity },
                     location, -1, -1);
}

MidiEvent MidiEvent::noteOff(int ticks, uint8_t channel, uint8_t pitch,
                             const SystemLocation &location)
{
    return MidiEvent(ticks, StatusByte::NoteOff + channel, { pitch, 127 },
                     location, -1, -1);
}

MidiEvent MidiEvent::volumeChange(int ticks, uint8_t channel, uint8_t level)
{
    return MidiEvent(ticks, StatusByte::ControlChange + channel,
                     { Controller::ChannelVolume, level }, SystemLocation(), -1,
                     -1);
}

MidiEvent MidiEvent::programChange(int ticks, uint8_t channel, uint8_t preset)
{
    return MidiEvent(ticks, StatusByte::ProgramChange + channel, { preset },
                     SystemLocation(), -1, -1);
}
