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
#include <exception>
#include <score/dynamic.h>
#include <score/generalmidi.h>
#include <cassert>

#ifdef __APPLE__
#include "midisoftwaresynth.h"
#endif

MidiOutputDevice::MidiOutputDevice() : myMidiOut(nullptr)
{
    // Initialize the OSX software synth.
#ifdef __APPLE__
    try
    {
        static MidiSoftwareSynth synth;
        synth.initialize();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    };
#endif

    myMaxVolumes.fill(Midi::MAX_MIDI_CHANNEL_VOLUME);
    myActiveVolumes.fill(static_cast<uint8_t>(VolumeLevel::fff));

    // Create all MIDI APIs supported on this platform.
    std::vector<RtMidi::Api> apis;
    RtMidi::getCompiledApi(apis);

    for (const RtMidi::Api &api : apis)
    {
        try
        {
            myMidiOuts.emplace_back(std::make_unique<RtMidiOut>(api));
        }
        catch (RtMidiError &e)
        {
            // Continue anyway, another API might work.
            e.printMessage();
        }
    }
}

MidiOutputDevice::~MidiOutputDevice()
{
    // Make sure there aren't any lingering notes.
    if (myMidiOut)
        stopAllNotes();
}

void
MidiOutputDevice::stopAllNotes()
{
    for (uint8_t channel = 0; channel < Midi::NUM_MIDI_CHANNELS_PER_PORT;
         ++channel)
    {
        // Turn off the pedal in case a "let ring" event was active.
        sendMidiMessage(ControlChange + channel, HoldPedal, 0);
        // Stop all notes.
        sendMidiMessage(ControlChange + channel, AllNotesOff, 0);
    }
}

void
MidiOutputDevice::sendMessage(const std::vector<uint8_t> &data)
{
    myMidiOut->sendMessage(&data);
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
    catch (RtMidiError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

bool
MidiOutputDevice::initialize(const std::string_view &preferred_api,
                             const std::string_view &preferred_port)
{
    if (myMidiOut)
        myMidiOut->closePort(); // Close any open ports.

    myMidiOut = nullptr;
    unsigned int port = 0;

    for (auto &&midi_out : myMidiOuts)
    {
        if (RtMidi::getApiName(midi_out->getCurrentApi()) != preferred_api)
            continue;

        for (unsigned int i = 0, n = midi_out->getPortCount(); i < n; ++i)
        {
            if (midi_out->getPortName(i) == preferred_port)
            {
                myMidiOut = midi_out.get();
                port = i;
                break;
            }
        }
    }

    // If there isn't an existing preference, just use the first port.
    if (!myMidiOut && (preferred_api.empty() && preferred_port.empty()) &&
        !myMidiOuts.empty() && myMidiOuts[0]->getPortCount())
    {
        myMidiOut = myMidiOuts[0].get();
    }

    if (!myMidiOut)
        return false;

    try
    {
        myMidiOut->openPort(port);
    }
    catch (RtMidiError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

size_t MidiOutputDevice::getApiCount()
{
    return myMidiOuts.size();
}

std::string
MidiOutputDevice::getApiName(size_t api)
{
    return RtMidi::getApiName(myMidiOuts[api]->getCurrentApi());
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

bool
MidiOutputDevice::setPitchBend(int channel, uint16_t amount)
{
    uint8_t lower_bits, upper_bits;
    Midi::splitIntoBytes(amount, lower_bits, upper_bits);

    return sendMidiMessage(PitchWheel + channel, lower_bits, upper_bits);
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
