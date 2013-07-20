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
  
#include <catch.hpp>

#include <boost/assign.hpp>

#include "tuning_fixtures.h"
#include "serialization_test.h"
#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>

TEST_CASE_METHOD(StandardTuningFixture, "PowerTabDocument/Tuning/GetSpelling", "")
{
    REQUIRE("E A D G B E" == tuning.GetSpelling());

    tuning.SetNote(0, midi::MIDI_NOTE_CSHARP0);
    REQUIRE("E A D G B C#" == tuning.GetSpelling());

    tuning.SetSharps(false);
    REQUIRE("E A D G B Db" == tuning.GetSpelling());
}

TEST_CASE("PowerTabDocument/Tuning/AssignmentAndEquality", "")
{
    Tuning tuning1, tuning2;

    tuning1.SetToStandard();
    REQUIRE(tuning1 != tuning2);

    tuning2 = tuning1;
    REQUIRE(tuning1 == tuning2);

    Tuning tuning3(tuning1);
    REQUIRE(tuning1 == tuning3);
}

TEST_CASE_METHOD(StandardTuningFixture, "PowerTabDocument/Tuning/SetTuningNotes", "")
{
    Tuning tuning2;

    using namespace midi;
    using namespace boost::assign;
    std::vector<uint8_t> notes;

    notes += MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3,
             MIDI_NOTE_D3, MIDI_NOTE_A2, MIDI_NOTE_E2;

    tuning2.SetTuningNotes(notes);

    REQUIRE(tuning2.IsSameTuning(tuning));

    notes.clear();
    notes += MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3;
    tuning2.SetTuningNotes(notes);

    REQUIRE(!tuning2.IsSameTuning(tuning));
}

TEST_CASE_METHOD(StandardTuningFixture, "PowerTabDocument/Tuning/IsOpenStringNote", "")
{
    REQUIRE(tuning.IsOpenStringNote(midi::MIDI_NOTE_E4));
    REQUIRE(tuning.IsOpenStringNote(midi::MIDI_NOTE_A2));
    REQUIRE(!tuning.IsOpenStringNote(midi::MIDI_NOTE_A3));
}

TEST_CASE_METHOD(StandardTuningFixture, "PowerTabDocument/Tuning/GetNoteRange", "")
{
    REQUIRE(std::make_pair(midi::MIDI_NOTE_F2, midi::MIDI_NOTE_F6) == tuning.GetNoteRange(1));
}

TEST_CASE_METHOD(StandardTuningFixture, "PowerTabDocument/Tuning/Serialization", "")
{
    testSerialization(tuning);
}
