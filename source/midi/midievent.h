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
    enum StatusByte : uint8_t
    {
        NoteOff = 0x80,
        NoteOn = 0x90,
        ControlChange = 0xb0,
        ProgramChange = 0xc0,
        PitchWheel = 0xe0,
        SysEx = 0xf0,
        MetaMessage = 0xff
    };

    inline bool operator<(const MidiEvent &other) const
    {
        return myTicks < other.myTicks;
    }

    int getTicks() const { return myTicks; }
    void setTicks(int ticks) { myTicks = ticks; }
    uint8_t getStatusByte() const { return myData[0]; }
    const std::vector<uint8_t> &getData() const { return myData; }
    const SystemLocation &getLocation() const { return myLocation; }

    bool isTempoChange() const;
    int getTempo() const;
    bool isProgramChange() const;
    bool isPositionChange() const;
    bool isNoteOnOff() const;
    uint8_t getChannel() const;

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
    static MidiEvent positionChange(int ticks, const SystemLocation &location);
    static std::vector<MidiEvent> pitchWheelRange(int ticks, uint8_t channel,
                                                  uint8_t semitones);

private:
    MidiEvent(int ticks, std::vector<uint8_t> data,
              const SystemLocation &location, int player, int instrument);

    int myTicks; // TODO - does this need to be 64-bit for absolute times?
    std::vector<uint8_t> myData;

    SystemLocation myLocation;
    int myPlayer;
    int myInstrument;
};

#endif
