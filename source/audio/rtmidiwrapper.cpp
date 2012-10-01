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
  
#include "rtmidiwrapper.h"

#include <audio/rtmidi/RtMidi.h>

RtMidiWrapper::RtMidiWrapper() :
    midiout(new RtMidiOut)
{
    for (int i = 0; i < 16; i++)
    {
        channelMaxVolumes[i] = 127;
        channelActiveVolumes[i] = 127;
    }
}

RtMidiWrapper::~RtMidiWrapper()
{
}

bool RtMidiWrapper::sendMidiMessage(uint8_t a, uint8_t b, uint8_t c)
{
    std::vector<uint8_t> message;

    message.push_back(a);

    if (b <= 127)
        message.push_back(b);

    if (c <= 127)
        message.push_back(c);

    try
    {
        midiout->sendMessage(&message);
    }
    catch (...)
    {
         return false;
    }

    return true;
}

bool RtMidiWrapper::initialize(uint32_t preferredPort)
{
    midiout->closePort(); // close any open ports

    uint32_t num_ports = midiout->getPortCount();

    if (num_ports<=0)
    {
        return false;
    }

    try
    {
        midiout->openPort(preferredPort);
    }
    catch (...)
    {
         return false;
    }

    return true;
}

uint32_t RtMidiWrapper::getPortCount()
{
    return midiout->getPortCount();
}

std::string RtMidiWrapper::getPortName(uint32_t port)
{
    return midiout->getPortName(port);
}

bool RtMidiWrapper::usePort(uint32_t port)
{
    try
    {
        midiout->closePort();
        midiout->openPort(port);
    }
    catch (...)
    {
         return false;
    }

    return true;
}

bool RtMidiWrapper::setPatch(uint8_t channel, uint8_t patch)
{
    if (patch > 127)
    {
        patch = 127;
    }

    // MIDI program change
    // first parameter is 0xC0-0xCF with C being the id and 0-F being the channel (0-15)
    // second parameter is the new patch (0-127)
    return sendMidiMessage(PROGRAM_CHANGE + channel, patch, -1);
}

bool RtMidiWrapper::setVolume (uint8_t channel, uint8_t volume)
{
    assert(volume <= 127);

    channelActiveVolumes[channel] = volume;

    return sendMidiMessage(CONTROL_CHANGE + channel, CHANNEL_VOLUME,
                           (volume / 127.0) * channelMaxVolumes[channel]);
}

bool RtMidiWrapper::setPan(uint8_t channel, uint8_t pan)
{
    if (pan > 127)
    { 
        pan = 127;
    }

    // MIDI control change
    // first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
    // second parameter is the control to change (0-127), 10 is channel pan
    // third parameter is the new pan (0-127)
    return sendMidiMessage(CONTROL_CHANGE + channel , PAN_CHANGE, pan);
}

bool RtMidiWrapper::setPitchBend (uint8_t channel, uint8_t bend)
{
    if (bend > 127)
        bend = 127;

    return sendMidiMessage(PITCH_WHEEL + channel, 0, bend);
}

bool RtMidiWrapper::playNote(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    if (pitch > 127)
    {
        pitch = 127;
    }

    if (velocity == 0)
    {
        velocity = 1;
    }
    else if (velocity > 127)
    {
        velocity = 127;
    }

    // MIDI note on
    // first parameter 0x90-9x9F with 9 being the id and 0-F being the channel (0-15)
    // second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
    // third parameter is the velocity of the note (1-127), 0 is not allowed, 64 would be no velocity
    return sendMidiMessage(NOTE_ON + channel, pitch, velocity);
}

bool RtMidiWrapper::stopNote(uint8_t channel, uint8_t pitch)
{
    if (pitch > 127)
        pitch=127;

    // MIDI note off
    // first parameter 0x80-9x8F with 8 being the id and 0-F being the channel (0-15)
    // second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
    return sendMidiMessage(NOTE_OFF + channel, pitch, 127);
}

bool RtMidiWrapper::setVibrato(uint8_t channel, uint8_t modulation)
{
    if (modulation > 127)
        modulation = 127;

    return sendMidiMessage(CONTROL_CHANGE + channel, MOD_WHEEL, modulation);
}

/// Turns sustain on or off for the specified channel
bool RtMidiWrapper::setSustain(uint8_t channel, bool sustainOn)
{
    const uint8_t value = sustainOn ? 127 : 0;
    
    return sendMidiMessage(CONTROL_CHANGE + channel, HOLD_PEDAL, value);
}

/// Sets the pitch bend range to the given number of semitones
void RtMidiWrapper::setPitchBendRange(uint8_t channel, uint8_t semiTones)
{
    sendMidiMessage(CONTROL_CHANGE + channel, RPN_MSB, 0);
    sendMidiMessage(CONTROL_CHANGE + channel, RPN_LSB, 0);
    sendMidiMessage(CONTROL_CHANGE + channel, DATA_ENTRY_COARSE, semiTones);
    sendMidiMessage(CONTROL_CHANGE + channel, DATA_ENTRY_FINE, 0);
}

/// Set the upper limit on a channel's volume
/// - The volume can then be adjusted within that range by dynamic symbols
void RtMidiWrapper::setChannelMaxVolume(uint8_t channel, uint8_t newMaxVolume)
{
    assert(newMaxVolume <= 127);

    const bool maxVolumeChanged = channelMaxVolumes[channel] != newMaxVolume;

    channelMaxVolumes[channel] = newMaxVolume;

    // If the new volume is different from the existing volume, send out a MIDI message
    if (maxVolumeChanged)
    {
        setVolume(channel, channelActiveVolumes[channel]);
    }
}
