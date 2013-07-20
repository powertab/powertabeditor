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

#include <actions/addtappedharmonic.h>
#include <score/note.h>
#include "actionfixture.h"

TEST_CASE_METHOD(ActionFixture, "Actions/AddTappedHarmonic", "")
{
    Note note;
    myLocation.getPosition()->insertNote(note);
    const int tappedFret = 17;

    AddTappedHarmonic action(myLocation, tappedFret);

    action.redo();
    REQUIRE(myLocation.getNote()->hasTappedHarmonic());
    REQUIRE(myLocation.getNote()->getTappedHarmonicFret() == tappedFret);

    action.undo();
    REQUIRE(!myLocation.getNote()->hasTappedHarmonic());
}
