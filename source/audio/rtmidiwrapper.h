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
  
#ifndef RTMIDIWRAPPER_H
#define RTMIDIWRAPPER_H

/**
// MIDI control change:
// first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
// second parameter is the control to change (0-127), (e.g. 7 is channel volume)
// third parameter is the new value (0-127)
**/

#include <string>
#include <map>
#include <boost/scoped_ptr.hpp>

class RtMidiOut;

class RtMidiWrapper
{
private:
    boost::scoped_ptr<RtMidiOut> midiout;
    bool sendMidiMessage(uint8_t a, uint8_t b, uint8_t c);

    std::map<uint8_t, uint8_t> channelMaxVolumes; ///< Maximum volume for each channel (as set in the mixer)
    std::map<uint8_t, uint8_t> channelActiveVolumes; ///< Volume of last active dynamic for each channel

public:
    RtMidiWrapper();
    ~RtMidiWrapper();

    bool initialize(uint32_t preferredPort = 0);
    void setPitchBendRange(uint8_t channel, uint8_t semiTones);
    uint32_t getPortCount();
    std::string getPortName(uint32_t port);
    bool usePort(uint32_t port);
    bool setPatch(uint8_t channel, uint8_t patch);
    bool setVolume(uint8_t channel, uint8_t volume);
    bool setPan(uint8_t channel, uint8_t pan); // pan in the range -100(left)-100(right)
    bool setPitchBend(uint8_t channel, uint8_t bend);
    bool playNote(uint8_t channel, uint8_t pitch, uint8_t velocity);
    bool stopNote(uint8_t channel, uint8_t pitch);
    bool setVibrato(uint8_t channel, uint8_t modulation);
    bool setSustain(uint8_t channel, bool sustainOn);

    void setChannelMaxVolume(uint8_t channel, uint8_t maxVolume);

    enum MidiMessage
    {
        NOTE_OFF = 128,
        NOTE_ON = 144,
        CONTROL_CHANGE = 176,
        PROGRAM_CHANGE = 192,
        PITCH_WHEEL = 224
    };

    enum ControlChanges
    {
        MOD_WHEEL = 1,
        DATA_ENTRY_COARSE = 6,
        CHANNEL_VOLUME = 7,
        PAN_CHANGE = 10,
        DATA_ENTRY_FINE = 38,
        HOLD_PEDAL = 64,
        RPN_LSB = 100,
        RPN_MSB = 101
    };
};

#endif // RTMIDIWRAPPER_H
