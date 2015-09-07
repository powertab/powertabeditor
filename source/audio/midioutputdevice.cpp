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
  
#include "midioutputdevice.h"

#include <RtMidi.h>
#include <score/dynamic.h>
#include <score/generalmidi.h>
#include <cassert>

MidiOutputDevice::MidiOutputDevice() : myMidiOut(nullptr)
{
    myMaxVolumes.fill(Midi::MAX_MIDI_CHANNEL_VOLUME);
    myActiveVolumes.fill(Dynamic::fff);

    // Create all MIDI APIs supported on this platform.
    std::vector<RtMidi::Api> apis;
    RtMidi::getCompiledApi(apis);

    for (const RtMidi::Api &api : apis)
    {
        try
        {
            myMidiOuts.emplace_back(new RtMidiOut(api));
        }
        catch (...)
        {
            // continue anyway, another api might work
            // found on mac that the Core API kept failing after repeated 
            // creations and the exceptions weren't caught
            // TODO investigate why.
        }
    }
}

MidiOutputDevice::~MidiOutputDevice()
{
}

void
MidiOutputDevice::sendMessage(const std::vector<uint8_t> &data)
{
    // FIXME - fix const correctness in RtMidi api.
    myMidiOut->sendMessage(const_cast<std::vector<uint8_t> *>(&data));
}

bool MidiOutputDevice::sendMidiMessage(unsigned char a, unsigned char b,
                                       unsigned char c)
{
    std::vector<uint8_t> message;

    message.push_back(a);

    if (b <= 127)
        message.push_back(b);

    if (c <= 127)
        message.push_back(c);

    try
    {
        myMidiOut->sendMessage(&message);
    }
    catch (...)
    {
         return false;
    }

    return true;
}

bool MidiOutputDevice::initialize(size_t preferredApi,
                                  unsigned int preferredPort)
{
    if (myMidiOut)
        myMidiOut->closePort(); // Close any open ports.

    if (preferredApi >= myMidiOuts.size())
        return false;

    myMidiOut = myMidiOuts[preferredApi].get();
    unsigned int num_ports = myMidiOut->getPortCount();

    if (num_ports == 0)
        return false;

    try
    {
        myMidiOut->openPort(preferredPort);
    }
    catch (...)
    {
         return false;
    }

    return true;
}

size_t MidiOutputDevice::getApiCount()
{
    return myMidiOuts.size();
}

unsigned int MidiOutputDevice::getPortCount(size_t api)
{
    assert(api < myMidiOuts.size() && "Programming error, api doesn't exist");
    return myMidiOuts[api]->getPortCount();
}

std::string MidiOutputDevice::getPortName(size_t api, unsigned int port)
{
    assert(api < myMidiOuts.size() && "Programming error, api doesn't exist");
    return myMidiOuts[api]->getPortName(port);
}

bool MidiOutputDevice::setPatch(int channel, uint8_t patch)
{
    if (patch > 127)
    {
        patch = 127;
    }

    // MIDI program change:
    // - first parameter is 0xC0-0xCF with C being the id and 0-F being the
    //   channel (0-15).
    // - second parameter is the new patch (0-127).
    return sendMidiMessage(ProgramChange + channel, patch, -1);
}

bool MidiOutputDevice::setVolume (int channel, uint8_t volume)
{
    assert(volume <= 127);

    myActiveVolumes[channel] = volume;

    return sendMidiMessage(
        ControlChange + channel, ChannelVolume,
        static_cast<int>((volume / 127.0) * myMaxVolumes[channel]));
}

bool MidiOutputDevice::setPan(int channel, uint8_t pan)
{
    if (pan > 127)
        pan = 127;

    // MIDI control change
    // first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
    // second parameter is the control to change (0-127), 10 is channel pan
    // third parameter is the new pan (0-127)
    return sendMidiMessage(ControlChange + channel, PanChange, pan);
}

bool MidiOutputDevice::setPitchBend (int channel, uint8_t bend)
{
    if (bend > 127)
        bend = 127;

    return sendMidiMessage(PitchWheel + channel, 0, bend);
}

bool MidiOutputDevice::playNote(int channel, uint8_t pitch, uint8_t velocity)
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
    return sendMidiMessage(NoteOn + channel, pitch, velocity);
}

bool MidiOutputDevice::stopNote(int channel, uint8_t pitch)
{
    if (pitch > 127)
        pitch=127;

    // MIDI note off
    // first parameter 0x80-9x8F with 8 being the id and 0-F being the channel (0-15)
    // second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
    return sendMidiMessage(NoteOff + channel, pitch, 127);
}

bool MidiOutputDevice::setVibrato(int channel, uint8_t modulation)
{
    if (modulation > 127)
        modulation = 127;

    return sendMidiMessage(ControlChange + channel, ModWheel, modulation);
}

bool MidiOutputDevice::setSustain(int channel, bool sustainOn)
{
    const uint8_t value = sustainOn ? 127 : 0;
    
    return sendMidiMessage(ControlChange + channel, HoldPedal, value);
}

void MidiOutputDevice::setPitchBendRange(int channel, uint8_t semiTones)
{
    sendMidiMessage(ControlChange + channel, RpnMsb, 0);
    sendMidiMessage(ControlChange + channel, RpnLsb, 0);
    sendMidiMessage(ControlChange + channel, DataEntryCoarse, semiTones);
    sendMidiMessage(ControlChange + channel, DataEntryFine, 0);
}

void MidiOutputDevice::setChannelMaxVolume(int channel, uint8_t newMaxVolume)
{
    assert(newMaxVolume <= 127);

    const bool maxVolumeChanged = myMaxVolumes[channel] != newMaxVolume;
    myMaxVolumes[channel] = newMaxVolume;

    // If the new volume is different from the existing volume, send out a MIDI message
    if (maxVolumeChanged)
        setVolume(channel, myActiveVolumes[channel]);
}
