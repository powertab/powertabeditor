/*
  * Copyright (C) 2013 Cameron White
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
  
#include <catch.hpp>

#include <score/generalmidi.h>
#include <score/tuning.h>
#include "test_serialization.h"

using namespace Score; 

TEST_CASE("Score/Tuning/GetSpelling", "")
{
    Tuning tuning;
    REQUIRE("E A D G B E" == tuning.getSpelling());

    tuning.setNote(0, Midi::MIDI_NOTE_CSHARP0);
    REQUIRE("E A D G B C#" == tuning.getSpelling());

    tuning.setSharps(false);
    REQUIRE("E A D G B Db" == tuning.getSpelling());
}

TEST_CASE("Score/Tuning/MusicNotationOffset", "")
{
    Tuning tuning;

    REQUIRE(tuning.getNote(0, true) == Midi::MIDI_NOTE_E4);

    tuning.setMusicNotationOffset(3);
    REQUIRE(tuning.getNote(0, true) == Midi::MIDI_NOTE_G4);
}

TEST_CASE("Score/Tuning/SetTuningNotes", "")
{
    Tuning tuning1, tuning2;

    tuning1.setNote(1, Midi::MIDI_NOTE_C2);

    std::vector<uint8_t> notes;
    notes.push_back(Midi::MIDI_NOTE_E4);
    notes.push_back(Midi::MIDI_NOTE_C2);
    notes.push_back(Midi::MIDI_NOTE_G3);
    notes.push_back(Midi::MIDI_NOTE_D3);
    notes.push_back(Midi::MIDI_NOTE_A2);
    notes.push_back(Midi::MIDI_NOTE_E2);
    tuning2.setNotes(notes);

    REQUIRE(tuning1 == tuning2);
}

TEST_CASE("Score/Tuning/Serialization", "")
{
    Tuning tuning;
    std::vector<uint8_t> notes;
    notes.push_back(Midi::MIDI_NOTE_E4);
    notes.push_back(Midi::MIDI_NOTE_C2);
    notes.push_back(Midi::MIDI_NOTE_G3);
    tuning.setNotes(notes);

    tuning.setMusicNotationOffset(3);
    tuning.setSharps(false);
    tuning.setCapo(7);

    Serialization::test(tuning);
}
