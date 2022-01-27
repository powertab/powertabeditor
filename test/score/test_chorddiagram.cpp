/*
  * Copyright (C) 2022 Cameron White
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

#include <doctest/doctest.h>

#include <score/chorddiagram.h>
#include "test_serialization.h"

TEST_CASE("Score/ChordDiagram/Serialization")
{
    ChordName name;
    name.setNoChord(true);
    name.setBrackets(true);
    name.setTonicVariation(ChordName::DoubleFlat);
    name.setTonicKey(ChordName::D);

    ChordDiagram diagram;
    diagram.setChordName(name);
    diagram.setTopFret(3);
    diagram.setFretNumbers({-1,-1, 3, 5, 4, 6});

    Serialization::test("chord_diagram", diagram);
}
