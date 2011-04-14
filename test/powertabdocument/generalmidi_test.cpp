#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/generalmidi.h>

BOOST_AUTO_TEST_CASE(GetMidiNoteText)
{
    BOOST_CHECK_EQUAL(midi::GetMidiNoteText(midi::MIDI_NOTE_C0, false), "C");
    BOOST_CHECK_EQUAL(midi::GetMidiNoteText(midi::MIDI_NOTE_FSHARP0, true), "F#");
    BOOST_CHECK_EQUAL(midi::GetMidiNoteText(midi::MIDI_NOTE_FSHARP0, false), "Gb");
}
