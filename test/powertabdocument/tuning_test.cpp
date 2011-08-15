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
#include <boost/assign.hpp>

#include "tuning_fixtures.h"

#include "serialization_test.h"
#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>

BOOST_FIXTURE_TEST_CASE(GetSpelling, StandardTuningFixture)
{
    BOOST_CHECK_EQUAL("E A D G B E", tuning.GetSpelling());

    tuning.SetNote(0, midi::MIDI_NOTE_CSHARP0);
    BOOST_CHECK_EQUAL("E A D G B C#", tuning.GetSpelling());

    tuning.SetSharps(false);
    BOOST_CHECK_EQUAL("E A D G B Db", tuning.GetSpelling());
}

BOOST_AUTO_TEST_CASE(AssignmentAndEquality)
{
    Tuning tuning1, tuning2;

    tuning1.SetToStandard();
    BOOST_CHECK(tuning1 != tuning2);

    tuning2 = tuning1;
    BOOST_CHECK(tuning1 == tuning2);

    Tuning tuning3(tuning1);
    BOOST_CHECK(tuning1 == tuning3);
}

BOOST_FIXTURE_TEST_CASE(SetTuningNotes, StandardTuningFixture)
{
    Tuning tuning2;

    using namespace midi;
    using namespace boost::assign;
    std::vector<uint8_t> notes;

    notes += MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3,
             MIDI_NOTE_D3, MIDI_NOTE_A2, MIDI_NOTE_E2;

    tuning2.SetTuningNotes(notes);

    BOOST_CHECK(tuning2.IsSameTuning(tuning));

    notes.clear();
    notes += MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3;
    tuning2.SetTuningNotes(notes);

    BOOST_CHECK(!tuning2.IsSameTuning(tuning));
}

BOOST_FIXTURE_TEST_CASE(IsOpenStringNote, StandardTuningFixture)
{
    BOOST_CHECK(tuning.IsOpenStringNote(midi::MIDI_NOTE_E4));
    BOOST_CHECK(tuning.IsOpenStringNote(midi::MIDI_NOTE_A2));
    BOOST_CHECK(!tuning.IsOpenStringNote(midi::MIDI_NOTE_A3));
}

BOOST_FIXTURE_TEST_CASE(GetNoteRange, StandardTuningFixture)
{
    BOOST_CHECK(std::make_pair(midi::MIDI_NOTE_F2, midi::MIDI_NOTE_F6) == tuning.GetNoteRange(1));
}

BOOST_FIXTURE_TEST_CASE(Serialization, StandardTuningFixture)
{
    testSerialization(tuning);
}
