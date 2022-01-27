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

#include "actionfixture.h"
#include <actions/chorddiagram.h>
#include <score/chorddiagram.h>

TEST_CASE("Actions/ChordDiagram")
{
    Score score;

    ChordDiagram diagram;
    diagram.setTopFret(2);

    AddChordDiagram action(score, diagram);

    action.redo();
    REQUIRE(score.getChordDiagrams().size() == 1);
    REQUIRE(score.getChordDiagrams()[0] == diagram);

    action.undo();
    REQUIRE(score.getChordDiagrams().size() == 0);

    action.redo();

    {
        ChordDiagram diagram2;
        diagram2.setTopFret(3);

        AddChordDiagram action(score, diagram2, 0);

        action.redo();
        REQUIRE(score.getChordDiagrams().size() == 2);
        REQUIRE(score.getChordDiagrams()[0] == diagram2);

        action.undo();
        REQUIRE(score.getChordDiagrams().size() == 1);
        REQUIRE(score.getChordDiagrams()[0] == diagram);
    }
}
