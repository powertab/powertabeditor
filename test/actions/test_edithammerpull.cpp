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

#include <actions/edithammerpull.h>
#include <powertabdocument/note.h>

TEST_CASE("Actions/EditHammerPull/Hammeron/Set", "")
{
    Note note;
    EditHammerPull action(&note, EditHammerPull::hammerOn);

    action.redo();
    REQUIRE(note.HasHammerOn());

    action.undo();
    REQUIRE(!note.HasHammerOn());

    REQUIRE(action.text() == "Set Hammer On");
}

TEST_CASE("Actions/EditHammerPull/Hammeron/Remove", "")
{
    Note note;
    note.SetHammerOn(true);

    EditHammerPull action(&note, EditHammerPull::hammerOn);

    action.redo();
    REQUIRE(!note.HasHammerOn());

    action.undo();
    REQUIRE(note.HasHammerOn());

    REQUIRE(action.text() == "Remove Hammer On");
}

TEST_CASE("Actions/EditHammerPull/Hammeron/ReplaceSlideWithHammeron", "")
{
    Note note;

    // Create a note with a slide.
    uint8_t slideType = Note::slideOutOfNone;
    int8_t steps = 0;
    note.SetSlideOutOf(Note::slideOutOfShiftSlide, 5);

    EditHammerPull action(&note, EditHammerPull::hammerOn);

    action.redo();
    REQUIRE(note.HasHammerOn());
    note.GetSlideOutOf(slideType, steps);
    REQUIRE(slideType == Note::slideOutOfNone);
    REQUIRE(steps == 0);

    action.undo();
    REQUIRE(!note.HasHammerOn());
    note.GetSlideOutOf(slideType, steps);
    REQUIRE(slideType == Note::slideOutOfShiftSlide);
    REQUIRE(steps == 5);
}

TEST_CASE("Actions/EditHammerPull/Pulloff/Set", "")
{
    Note note;
    EditHammerPull action(&note, EditHammerPull::pullOff);

    action.redo();
    REQUIRE(note.HasPullOff());

    action.undo();
    REQUIRE(!note.HasPullOff());

    REQUIRE(action.text() == "Set Pull Off");
}

TEST_CASE("Actions/EditHammerPull/Pulloff/Remove", "")
{
    Note note;
    note.SetPullOff(true);

    EditHammerPull action(&note, EditHammerPull::pullOff);

    action.redo();
    REQUIRE(!note.HasPullOff());

    action.undo();
    REQUIRE(note.HasPullOff());

    REQUIRE(action.text() == "Remove Pull Off");
}

TEST_CASE("Actions/EditHammerPull/Pulloff/ReplaceSlideWithPulloff", "")
{
    Note note;

    // Create a note with a slide.
    uint8_t slideType = Note::slideOutOfNone;
    int8_t steps = 0;
    note.SetSlideOutOf(Note::slideOutOfShiftSlide, -5);

    EditHammerPull action(&note, EditHammerPull::pullOff);

    action.redo();
    REQUIRE(note.HasPullOff());
    note.GetSlideOutOf(slideType, steps);
    REQUIRE(slideType == Note::slideOutOfNone);
    REQUIRE(steps == 0);

    action.undo();
    REQUIRE(!note.HasPullOff());
    note.GetSlideOutOf(slideType, steps);
    REQUIRE(slideType == Note::slideOutOfShiftSlide);
    REQUIRE(steps == -5);
}
