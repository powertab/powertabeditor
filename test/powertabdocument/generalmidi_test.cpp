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

#include <powertabdocument/generalmidi.h>

TEST_CASE("PowerTabDocument/GeneralMidi/GetMidiNoteTextSimple", "")
{
    using namespace midi;

    REQUIRE(GetMidiNoteTextSimple(MIDI_NOTE_C0, false) == "C");
    REQUIRE(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, true) == "F#");
    REQUIRE(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, false) == "Gb");
}

TEST_CASE("PowerTabDocument/GeneralMidi/GetMidiNoteText", "")
{
    using namespace midi;

    REQUIRE(GetMidiNoteTextSimple(MIDI_NOTE_C0, false) == "C");
    REQUIRE(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, true) == "F#");
    REQUIRE(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, false) == "Gb");

    REQUIRE(GetMidiNoteText(MIDI_NOTE_G3, true, 2) == "G");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_F3, true, 2) == "F=");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_FSHARP3, true, 2) == "F");

    REQUIRE(GetMidiNoteText(MIDI_NOTE_G3, true, 2, true) == "G=");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_F3, true, 2, true) == "F=");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_FSHARP3, true, 2, true) == "F#");

    REQUIRE(GetMidiNoteText(MIDI_NOTE_C3, true, 0) == "C");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_ASHARP3, true, 2) == "Bb");

    REQUIRE(GetMidiNoteText(MIDI_NOTE_ASHARP3, false, 2) == "B");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_E3, false, 2) == "E=");
    REQUIRE(GetMidiNoteText(MIDI_NOTE_DSHARP3, false, 2) == "E");

    REQUIRE(GetMidiNoteText(MIDI_NOTE_G3, true, 5, false) == "G=");
}
