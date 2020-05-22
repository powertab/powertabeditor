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

#ifndef AUDIO_MIDISOFTWARESYNTH_H
#define AUDIO_MIDISOFTWARESYNTH_H

#include <AudioToolbox/AUGraph.h>
#include <AudioToolbox/AudioUnit.h>
#include <CoreMIDI/MIDIServices.h>

#include <optional>

/// AudioUnit-based software synth. This is registered as a MIDI destination so
/// that we can use it via RtMidi if the user doesn't have any other MIDI
/// outputs.
class MidiSoftwareSynth
{
public:
    ~MidiSoftwareSynth();

    void initialize();

private:
    /// Callback invoked when MIDI packets arrive.
    static void readProc(const MIDIPacketList *packets, void *readProcRefCon,
                         void *);

    std::optional<MIDIClientRef> myClient;
    std::optional<MIDIEndpointRef> myEndpoint;

    std::optional<AUGraph> myGraph;
    AudioUnit mySynthesizer = nullptr;
};

#endif
