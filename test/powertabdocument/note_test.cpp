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

#include <boost/function.hpp>
#include "serialization_test.h"
#include <powertabdocument/note.h>

// helper function for testing note properties
void testNoteProperty(boost::function<bool (Note*, bool)> setProperty,
                      boost::function<bool (const Note*)> getProperty)
{
    Note note;

    setProperty(&note, true);
    REQUIRE(getProperty(&note) == true);

    setProperty(&note, false);
    REQUIRE(getProperty(&note) == false);
}

TEST_CASE("PowerTabDocument/Note/ConstructionAndAssignment", "")
{
    Note note1(4, 5);
    note1.SetTrill(7); // add some complex symbols
    note1.SetSlideInto(Note::slideIntoFromBelow);

    Note note2;

    REQUIRE(note1 != note2);

    note2 = note1;

    REQUIRE(note1 == note2);

    Note note3(4, 5);
    // add complex symbols in a different order
    note3.SetSlideInto(Note::slideIntoFromBelow);
    note3.SetTrill(7);

    REQUIRE(note1 == note3);
}

TEST_CASE("PowerTabDocument/Note/Serialization", "")
{
    Note note(4, 5);
    note.SetTrill(7);
    note.SetSlideOutOf(Note::slideOutOfDownwards, 3);

    testSerialization(note);
}

TEST_CASE("PowerTabDocument/Note/Trills", "")
{
    Note note;
    note.SetTrill(3);

    REQUIRE(note.HasTrill());

    uint8_t trill;
    note.GetTrill(trill);
    REQUIRE(trill == 3);

    note.ClearTrill();
    REQUIRE(!note.HasTrill());
}

TEST_CASE("PowerTabDocument/Note/Octave8va", "")
{
    testNoteProperty(&Note::SetOctave8va, &Note::IsOctave8va);
}

TEST_CASE("PowerTabDocument/Note/Octave15va", "")
{
    testNoteProperty(&Note::SetOctave15ma, &Note::IsOctave15ma);
}

TEST_CASE("PowerTabDocument/Note/Octave8vb", "")
{
    testNoteProperty(&Note::SetOctave8vb, &Note::IsOctave8vb);
}

TEST_CASE("PowerTabDocument/Note/Octave15mb", "")
{
    testNoteProperty(&Note::SetOctave15mb, &Note::IsOctave15mb);
}

TEST_CASE("PowerTabDocument/Note/BendText", "")
{
    REQUIRE(Note::GetBendText(0) == "Standard");
    REQUIRE(Note::GetBendText(4) == "Full");
    REQUIRE(Note::GetBendText(8) == "2");
    REQUIRE(Note::GetBendText(2) == "1/2");
    REQUIRE(Note::GetBendText(9) == "2 1/4");
    REQUIRE(Note::GetBendText(10) == "2 1/2");
    REQUIRE(Note::GetBendText(11) == "2 3/4");
}

TEST_CASE("PowerTabDocument/Note/ToggleSlide", "")
{
    Note note;
    note.SetSlideOutOf(Note::slideOutOfShiftSlide, -2);
    REQUIRE(note.HasShiftSlide());

    note.SetSlideOutOf(Note::slideOutOfLegatoSlide, -2);
    REQUIRE(note.HasLegatoSlide());
}
