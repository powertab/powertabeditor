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

#include <cassert>

enum Controller : uint8_t
{
    ModWheel = 0x01,
    DataEntryCoarse = 0x06,
    ChannelVolume = 0x07,
    DataEntryFine = 0x26,
    HoldPedal = 0x40,
    RpnLsb = 0x64,
    RpnMsb = 0x65
};

enum MetaType : uint8_t
{
    TrackEnd = 0x2f,
    SetTempo = 0x51
};

static const uint8_t theSysExMsgEnd = 0xf7;
static const uint8_t theSysExManufacturerId = 0x7d;
static const uint8_t theChannelMask = 0x0f;
static const uint8_t theStatusByteMask = ~theChannelMask;

MidiEvent::MidiEvent(int ticks, std::vector<uint8_t> data,
                     const SystemLocation &location, int player, int instrument)
    : myTicks(ticks),
      myData(std::move(data)),
      myLocation(location),
      myPlayer(player),
      myInstrument(instrument)
{
}

MidiEvent MidiEvent::endOfTrack(int ticks)
{
    return MidiEvent(ticks, { StatusByte::MetaMessage, MetaType::TrackEnd, 0 },
                     SystemLocation(), -1, -1);
}

bool MidiEvent::isTempoChange() const
{
    return getStatusByte() == StatusByte::MetaMessage &&
           myData[1] == MetaType::SetTempo;
}

int MidiEvent::getTempo() const
{
    assert(isTempoChange());
    assert(myData[2] == 3);
    return myData[5] + (myData[4] << 8) + (myData[3] << 16);
}

bool MidiEvent::isProgramChange() const
{
    return (getStatusByte() & theStatusByteMask) == StatusByte::ProgramChange;
}

MidiEvent MidiEvent::setTempo(int ticks, int microseconds)
{
    const uint32_t val = microseconds;
    return MidiEvent(ticks, { StatusByte::MetaMessage,
                              static_cast<uint8_t>(MetaType::SetTempo), 3,
                              static_cast<uint8_t>((val >> 16) & 0xff),
                              static_cast<uint8_t>((val >> 8) & 0xff),
                              static_cast<uint8_t>(val & 0xff) },
                     SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::noteOn(int ticks, uint8_t channel, uint8_t pitch,
                            uint8_t velocity, const SystemLocation &location)
{
    return MidiEvent(
        ticks,
        { static_cast<uint8_t>(StatusByte::NoteOn + channel), pitch, velocity },
        location, -1, -1);
}

MidiEvent MidiEvent::noteOff(int ticks, uint8_t channel, uint8_t pitch,
                             const SystemLocation &location)
{
    return MidiEvent(
        ticks,
        { static_cast<uint8_t>(StatusByte::NoteOff + channel), pitch, 127 },
        location, -1, -1);
}

MidiEvent MidiEvent::volumeChange(int ticks, uint8_t channel, uint8_t level)
{
    return MidiEvent(
        ticks, { static_cast<uint8_t>(StatusByte::ControlChange + channel),
                 Controller::ChannelVolume, level },
        SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::programChange(int ticks, uint8_t channel, uint8_t preset)
{
    return MidiEvent(
        ticks,
        { static_cast<uint8_t>(StatusByte::ProgramChange + channel), preset },
        SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::modWheel(int ticks, uint8_t channel, uint8_t width)
{
    return MidiEvent(
        ticks, { static_cast<uint8_t>(StatusByte::ControlChange + channel),
                 Controller::ModWheel, width },
        SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::holdPedal(int ticks, uint8_t channel, bool enabled)
{
    return MidiEvent(
        ticks,
        { static_cast<uint8_t>(StatusByte::ControlChange + channel),
          Controller::HoldPedal, static_cast<uint8_t>(enabled ? 127 : 0) },
        SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::pitchWheel(int ticks, uint8_t channel, uint8_t amount)
{
    return MidiEvent(
        ticks,
        { static_cast<uint8_t>(StatusByte::PitchWheel + channel), 0, amount },
        SystemLocation(), -1, -1);
}

MidiEvent MidiEvent::positionChange(int ticks, const SystemLocation &location)
{
    return MidiEvent(
        ticks, { StatusByte::SysEx, theSysExManufacturerId, theSysExMsgEnd },
        location, -1, -1);
}

bool MidiEvent::isPositionChange() const
{
    return getStatusByte() == StatusByte::SysEx &&
           myData[1] == theSysExManufacturerId;
}

bool MidiEvent::isNoteOnOff() const
{
    return (getStatusByte() & theStatusByteMask) == StatusByte::NoteOn ||
           (getStatusByte() & theStatusByteMask) == StatusByte::NoteOff;
}

uint8_t MidiEvent::getChannel() const
{
    return getStatusByte() & theChannelMask;
}

std::vector<MidiEvent> MidiEvent::pitchWheelRange(int ticks, uint8_t channel,
                                                  uint8_t semitones)
{
    return {
        MidiEvent(ticks,
                  { static_cast<uint8_t>(StatusByte::ControlChange + channel),
                    Controller::RpnMsb, 0 },
                  SystemLocation(), -1, -1),
        MidiEvent(ticks,
                  { static_cast<uint8_t>(StatusByte::ControlChange + channel),
                    Controller::RpnLsb, 0 },
                  SystemLocation(), -1, -1),
        MidiEvent(ticks,
                  { static_cast<uint8_t>(StatusByte::ControlChange + channel),
                    Controller::DataEntryCoarse, semitones },
                  SystemLocation(), -1, -1),
        MidiEvent(ticks,
                  { static_cast<uint8_t>(StatusByte::ControlChange + channel),
                    Controller::DataEntryFine, 0 },
                  SystemLocation(), -1, -1),
    };
}
