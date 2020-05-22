/*
  * Copyright (C) 2020 Cameron White
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

#include "midisoftwaresynth.h"

#include <MacTypes.h>

#include <memory>
#include <type_traits>

/// RAII wrapper for CFStringRef.
struct CFStringDeleter
{
    void operator()(CFStringRef p)
    {
        if (p)
            CFRelease(p);
    }
};

/// RAII wrapper for CFStringRef.
using CFStringHandle =
    std::unique_ptr<std::remove_pointer<CFStringRef>::type, CFStringDeleter>;

MidiSoftwareSynth::~MidiSoftwareSynth()
{
    if (myGraph)
        DisposeAUGraph(*myGraph);

    if (myEndpoint)
        MIDIEndpointDispose(*myEndpoint);
    if (myClient)
        MIDIClientDispose(*myClient);
}

void MidiSoftwareSynth::initialize()
{
    if (myClient)
        return;

    // Create the MIDI client.
    {
        MIDIClientRef client;
        CFStringHandle name(CFStringCreateWithCString(
            nullptr, "Power Tab Editor", kCFStringEncodingASCII));
        OSStatus result =
            MIDIClientCreate(name.get(), nullptr, nullptr, &client);
        if (result != noErr)
            throw std::runtime_error("Failed to create MIDI client");

        myClient = client;
    }

    // Create the MIDI endpoint.
    {
        MIDIEndpointRef endpoint;
        CFStringHandle name(CFStringCreateWithCString(
            nullptr, "Power Tab Software Synth", kCFStringEncodingASCII));
        OSStatus result = MIDIDestinationCreate(*myClient, name.get(),
                                                &readProc, this, &endpoint);
        if (result != noErr)
            throw std::runtime_error("Failed to create MIDI client");

        myEndpoint = endpoint;
    }

    // Set up AudioUnit synth.
    {
        AUGraph graph;
        if (NewAUGraph(&graph) != noErr)
            throw std::runtime_error("Failed to create audio graph.");

        myGraph = graph;
    }

    {
        AudioComponentDescription cd;
        cd.componentManufacturer = kAudioUnitManufacturer_Apple;
        cd.componentFlags = 0;
        cd.componentFlagsMask = 0;

        // Create the AU synthesizer (make audio from midi). Owned by the
        // graph.
        AUNode synthNode;
        cd.componentType = kAudioUnitType_MusicDevice;
        cd.componentSubType = kAudioUnitSubType_DLSSynth;

        if (AUGraphAddNode(*myGraph, &cd, &synthNode) != noErr)
            throw std::runtime_error("Failed to create synth node.");

        // Create the Peak Limiter (prevents erm peaks!)
        AUNode limiterNode;
        cd.componentType = kAudioUnitType_Effect;
        cd.componentSubType = kAudioUnitSubType_PeakLimiter;

        if (AUGraphAddNode(*myGraph, &cd, &limiterNode) != noErr)
            throw std::runtime_error("Failed to create limiter node.");

        // Audio output node (e.g. speakers).
        AUNode outNode;
        cd.componentType = kAudioUnitType_Output;
        cd.componentSubType = kAudioUnitSubType_DefaultOutput;

        if (AUGraphAddNode(*myGraph, &cd, &outNode) != noErr)
            throw std::runtime_error("Failed to create out node.");

        // Initialize and connect the audio graph.
        if (AUGraphOpen(*myGraph) != noErr)
        {
            throw std::runtime_error("Failed to open graph.");
        }
        else if (AUGraphConnectNodeInput(*myGraph, synthNode, 0, limiterNode,
                                         0) != noErr)
        {
            throw std::runtime_error("Failed to connect synth to limiter.");
        }
        else if (AUGraphConnectNodeInput(*myGraph, limiterNode, 0, outNode,
                                         0) != noErr)
        {
            throw std::runtime_error("Failed to connect limiter to output.");
        }
        else if (AUGraphInitialize(*myGraph) != noErr)
        {
            throw std::runtime_error("Failed to initialize graph.");
        }

        if (AUGraphNodeInfo(*myGraph, synthNode, 0, &mySynthesizer) != noErr)
            throw std::runtime_error("Failed to cache synthesizer.");

        if (AUGraphStart(*myGraph) != noErr)
            throw std::runtime_error("Failed to start synthesizer.");
    }
}

void MidiSoftwareSynth::readProc(const MIDIPacketList *packets,
                                 void *readProcRefCon, void *)
{
    auto me = static_cast<const MidiSoftwareSynth *>(readProcRefCon);

    const MIDIPacket *packet = &packets->packet[0];
    for (UInt32 i = 0, n = packets->numPackets; i < n; ++i)
    {
        // Forward the data to AudioUnit. We don't expect to have any long
        // sysex messages etc.
        UInt32 statusByte = packet->data[0];
        UInt32 dataByte1 = packet->length > 1 ? packet->data[1] : 0;
        UInt32 dataByte2 = packet->length > 2 ? packet->data[2] : 0;
        UInt32 offsetSampleFrame = 0;

        if (MusicDeviceMIDIEvent(me->mySynthesizer, statusByte, dataByte1,
                                 dataByte2, offsetSampleFrame) != noErr)
        {
            throw std::runtime_error("Failed to send message to synthesizer.");
        }

        packet = MIDIPacketNext(packet);
    }
}
