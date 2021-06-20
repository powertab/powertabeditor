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
  
#ifndef MIDI_MIDIFILE_H
#define MIDI_MIDIFILE_H

#include <midi/midieventlist.h>

#include <cstdint>
#include <vector>

class Barline;
class ConstScoreLocation;
class RepeatController;
class Score;
class Staff;
class System;
class SystemLocation;
class Voice;

class MidiFile
{
public:
    struct LoadOptions
    {
        LoadOptions()
            : myVibratoStrength(0),
              myWideVibratoStrength(0),
              myEnableMetronome(false),
              myStrongAccentVel(0),
              myWeakAccentVel(0),
              myMetronomePreset(0),
              myRecordPositionChanges(false)
        {
        }

        uint8_t myVibratoStrength;
        uint8_t myWideVibratoStrength;
        bool myEnableMetronome;
        uint8_t myStrongAccentVel;
        uint8_t myWeakAccentVel;
        uint8_t myMetronomePreset;
        bool myRecordPositionChanges;
    };

    MidiFile();

    void load(const Score &score, const LoadOptions &options);
    void loadSingleNote(const Score &score, const ConstScoreLocation &location,
                        const LoadOptions &options);

    int getTicksPerBeat() const { return myTicksPerBeat; }
    std::vector<MidiEventList> &getTracks() { return myTracks; }
    const std::vector<MidiEventList> &getTracks() const { return myTracks; }

private:
    int generateMetronome(MidiEventList &event_list, int current_tick,
                          const System &system, const Barline &current_bar,
                          const Barline &next_bar,
                          const SystemLocation &location,
                          const LoadOptions &options);

    Midi::Tempo addTempoEvent(MidiEventList &event_list, int current_tick,
                              Midi::Tempo current_tempo, const Score &score,
                              const SystemLocation &location,
                              const RepeatController &repeat_controller,
                              int bar_start, int bar_end);

    int addEventsForBar(std::vector<MidiEventList> &tracks,
                        uint8_t &active_bend, int current_tick,
                        Midi::Tempo current_tempo, const Score &score,
                        const System &system, int system_index,
                        const Staff &staff, int staff_index, const Voice &voice,
                        int voice_index, int bar_start, int bar_end,
                        const LoadOptions &options);

    int myTicksPerBeat;
    std::vector<MidiEventList> myTracks;
};

#endif
