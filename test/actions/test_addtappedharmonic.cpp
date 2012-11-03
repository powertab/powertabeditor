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

#include <actions/addtappedharmonic.h>
#include <powertabdocument/note.h>

TEST_CASE("Actions/AddTappedHarmonic", "")
{
    Note note;
    const uint8_t requestedTappedFret = 17;
    uint8_t currentTappedFret = 0;

    AddTappedHarmonic action(&note, requestedTappedFret);

    action.redo();
    REQUIRE(note.HasTappedHarmonic());
    note.GetTappedHarmonic(currentTappedFret);
    REQUIRE(currentTappedFret == requestedTappedFret);

    action.undo();
    REQUIRE(!note.HasTappedHarmonic());
}
