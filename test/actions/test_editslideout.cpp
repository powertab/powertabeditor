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

#include <actions/editslideout.h>
#include <powertabdocument/note.h>

TEST_CASE("Actions/EditSlideOut", "")
{
    Note note;
    uint8_t slideType = Note::slideOutOfNone;
    int8_t steps = 0;
    EditSlideOut action(&note, Note::slideOutOfLegatoSlide, 5);

    action.redo();
    note.GetSlideOutOf(slideType, steps);
    REQUIRE(slideType == Note::slideOutOfLegatoSlide);
    REQUIRE(steps == 5);

    action.undo();
    note.GetSlideOutOf(slideType, steps);
    REQUIRE(slideType == Note::slideOutOfNone);
    REQUIRE(steps == 0);

    REQUIRE(action.text() == "Legato Slide");
}

