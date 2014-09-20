/*
  * Copyright (C) 2012 Cameron White
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

#include <actions/edittabnumber.h>
#include <score/note.h>
#include "actionfixture.h"

TEST_CASE_METHOD(ActionFixture, "Actions/EditTabNumber", "")
{
    myLocation.getNote()->setFretNumber(5);
    myLocation.getNote()->setTappedHarmonicFret(8);

    EditTabNumber action(myLocation, 8);

    action.redo();
    REQUIRE(myLocation.getNote()->getFretNumber() == 8);
    REQUIRE(myLocation.getNote()->getTappedHarmonicFret() == 11);

    action.undo();
    REQUIRE(myLocation.getNote()->getFretNumber() == 5);
    REQUIRE(myLocation.getNote()->getTappedHarmonicFret() == 8);
}

TEST_CASE_METHOD(ActionFixture, "Actions/EditTabNumber/HighTappedHarmonic", "")
{
    myLocation.getNote()->setFretNumber(5);
    myLocation.getNote()->setTappedHarmonicFret(29);

    EditTabNumber action(myLocation, 8);

    action.redo();
    REQUIRE(myLocation.getNote()->getFretNumber() == 8);
    REQUIRE(!myLocation.getNote()->hasTappedHarmonic());

    action.undo();
    REQUIRE(myLocation.getNote()->getFretNumber() == 5);
    REQUIRE(myLocation.getNote()->getTappedHarmonicFret() == 29);
}
