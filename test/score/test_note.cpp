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

#include <score/note.h>
#include "test_serialization.h"

TEST_CASE("Score/Note/SimpleProperties", "")
{
    Note note;

    REQUIRE(!note.hasProperty(Note::HammerOn));
    note.setProperty(Note::HammerOn, true);
    REQUIRE(note.hasProperty(Note::HammerOn));
}

TEST_CASE("Score/Note/Trill", "")
{
    Note note;

    REQUIRE(!note.hasTrill());
    REQUIRE_THROWS(note.getTrilledFret());

    note.setTrilledFret(5);
    REQUIRE(note.hasTrill());
    REQUIRE(note.getTrilledFret() == 5);

    note.clearTrill();
    REQUIRE(!note.hasTrill());
    REQUIRE_THROWS(note.getTrilledFret());
}

TEST_CASE("Score/Note/TappedHarmonic", "")
{
    Note note;

    REQUIRE(!note.hasTappedHarmonic());
    REQUIRE_THROWS(note.getTappedHarmonicFret());

    note.setTappedHarmonicFret(5);
    REQUIRE(note.hasTappedHarmonic());
    REQUIRE(note.getTappedHarmonicFret() == 5);

    note.clearTappedHarmonic();
    REQUIRE(!note.hasTappedHarmonic());
    REQUIRE_THROWS(note.getTappedHarmonicFret());
}

TEST_CASE("Score/Note/Serialization", "")
{
    Note note(3, 12);
    note.setProperty(Note::Octave15ma);

    Serialization::test(note);
}
