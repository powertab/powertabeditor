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
  
#include <catch2/catch.hpp>

#include <score/chordname.h>
#include <util/tostring.h>
#include "test_serialization.h"

TEST_CASE("Score/ChordName/Serialization", "")
{
    ChordName name;
    name.setNoChord(true);
    name.setBrackets(true);
    name.setTonicVariation(ChordName::DoubleFlat);
    name.setTonicKey(ChordName::D);

    Serialization::test("chord_name", name);
}

TEST_CASE("Score/ChordName/ToString", "")
{
    ChordName chord;

    chord.setNoChord(true);
    REQUIRE(Util::toString(chord) == "N.C.");

    chord.setBrackets(true);
    chord.setTonicKey(ChordName::F);
    chord.setTonicVariation(ChordName::Flat);
    chord.setBassKey(ChordName::F);
    chord.setBassVariation(ChordName::Flat);
    chord.setFormula(ChordName::Major7th);
    REQUIRE(Util::toString(chord) == "N.C.(Fbmaj7)");

    chord.setModification(ChordName::Extended11th);
    REQUIRE(Util::toString(chord) == "N.C.(Fbmaj11)");
    chord.setModification(ChordName::Extended11th, false);

    chord.setNoChord(false);
    chord.setTonicVariation(ChordName::Sharp);
    chord.setBassKey(ChordName::C);
    chord.setBassVariation(ChordName::Sharp);
    chord.setModification(ChordName::Flatted9th);
    REQUIRE(Util::toString(chord) == "(F#maj7b9/C#)");
}
