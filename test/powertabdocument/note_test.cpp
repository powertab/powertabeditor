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

#include "serialization_test.h"
#include <powertabdocument/note.h>
#include <functional>

// helper function for testing note properties
void testNoteProperty(std::function<bool (Note*, bool)> setProperty,
                      std::function<bool (const Note*)> getProperty)
{
    Note note;

    setProperty(&note, true);
    BOOST_CHECK_EQUAL(getProperty(&note), true);

    setProperty(&note, false);
    BOOST_CHECK_EQUAL(getProperty(&note), false);
}

BOOST_AUTO_TEST_SUITE(NoteTest)

    BOOST_AUTO_TEST_CASE(ConstructionAndAssignment)
    {
        Note note1(4, 5);
        note1.SetTrill(7); // add some complex symbols
        note1.SetSlideInto(Note::slideIntoFromBelow);

        Note note2;

        BOOST_CHECK(note1 != note2);

        note2 = note1;

        BOOST_CHECK(note1 == note2);

        Note note3(4, 5);
        // add complex symbols in a different order
        note3.SetSlideInto(Note::slideIntoFromBelow);
        note3.SetTrill(7);

        BOOST_CHECK(note1 == note3);
    }

    BOOST_AUTO_TEST_CASE(Serialization)
    {
        Note note(4, 5);
        note.SetTrill(7);
        note.SetSlideOutOf(Note::slideOutOfDownwards, 3);

        testSerialization(note);
    }

    BOOST_AUTO_TEST_CASE(Trill)
    {
        Note note;
        note.SetTrill(3);

        BOOST_CHECK(note.HasTrill());

        uint8_t trill;
        note.GetTrill(trill);
        BOOST_CHECK_EQUAL(trill, 3);

        note.ClearTrill();
        BOOST_CHECK(!note.HasTrill());
    }

    BOOST_AUTO_TEST_CASE(Octave8va)
    {
        testNoteProperty(&Note::SetOctave8va, &Note::IsOctave8va);
    }

    BOOST_AUTO_TEST_CASE(Octave15ma)
    {
        testNoteProperty(&Note::SetOctave15ma, &Note::IsOctave15ma);
    }

    BOOST_AUTO_TEST_CASE(Octave8vb)
    {
        testNoteProperty(&Note::SetOctave8vb, &Note::IsOctave8vb);
    }

    BOOST_AUTO_TEST_CASE(Octave15mb)
    {
        testNoteProperty(&Note::SetOctave15mb, &Note::IsOctave15mb);
    }

BOOST_AUTO_TEST_SUITE_END()

