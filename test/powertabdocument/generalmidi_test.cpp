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
  
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/generalmidi.h>

BOOST_AUTO_TEST_CASE(TestGetMidiNoteTextSimple)
{
    using namespace midi;

    BOOST_CHECK_EQUAL(GetMidiNoteTextSimple(MIDI_NOTE_C0, false), "C");
    BOOST_CHECK_EQUAL(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, true), "F#");
    BOOST_CHECK_EQUAL(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, false), "Gb");
}

BOOST_AUTO_TEST_CASE(TestGetMidiNoteText)
{
    using namespace midi;

    BOOST_CHECK_EQUAL(GetMidiNoteTextSimple(MIDI_NOTE_C0, false), "C");
    BOOST_CHECK_EQUAL(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, true), "F#");
    BOOST_CHECK_EQUAL(GetMidiNoteTextSimple(MIDI_NOTE_FSHARP0, false), "Gb");

    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_G3, true, 2), "G");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_F3, true, 2), "F=");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_FSHARP3, true, 2), "F");

    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_G3, true, 2, true), "G=");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_F3, true, 2, true), "F=");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_FSHARP3, true, 2, true), "F#");

    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_C3, true, 0), "C");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_ASHARP3, true, 2), "Bb");

    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_ASHARP3, false, 2), "B");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_E3, false, 2), "E=");
    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_DSHARP3, false, 2), "E");

    BOOST_CHECK_EQUAL(GetMidiNoteText(MIDI_NOTE_G3, true, 5, false), "G=");
}
