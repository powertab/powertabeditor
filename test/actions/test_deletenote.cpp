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

#include <actions/deletenote.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

TEST_CASE("Actions/DeleteNote/CanDeleteNote", "")
{
    REQUIRE(!DeleteNote::canExecute(NULL, 2));

    Position position;
    REQUIRE(!DeleteNote::canExecute(&position, 3));

    position.InsertNote(new Note(3, 12));
    REQUIRE(DeleteNote::canExecute(&position, 3));
    REQUIRE(!DeleteNote::canExecute(&position, 2));
}

TEST_CASE("Actions/DeleteNote/DeleteNote", "")
{
    const uint32_t string1 = 2, string2 = 4;
    const uint8_t fret1 = 12, fret2 = 13;

    boost::shared_ptr<Staff> staff(new Staff);
    Position* position = new Position;
    position->InsertNote(new Note(string1, fret1));
    position->InsertNote(new Note(string2, fret2));
    staff->InsertPosition(0, position);

    DeleteNote action(staff, /* voice */ 0, position, string1, true);

    action.redo();
    REQUIRE(position->GetNoteCount() == 1);
    REQUIRE(!position->GetNoteByString(string1));
    REQUIRE(position->GetNoteByString(string2));

    action.undo();
    REQUIRE(position->GetNoteCount() == 2);
    REQUIRE(position->GetNoteByString(string1));
    REQUIRE(position->GetNoteByString(string2));
    REQUIRE(position->GetNoteByString(string1)->GetFretNumber() == fret1);
    REQUIRE(position->GetNoteByString(string2)->GetFretNumber() == fret2);
}

TEST_CASE("Actions/DeleteNote/DeleteLastNote", "")
{
    boost::shared_ptr<Staff> staff(new Staff);
    Position* position = new Position;
    position->InsertNote(new Note(2, 12));
    staff->InsertPosition(0, position);

    DeleteNote action(staff, /* voice */ 0, position, 2, true);

    action.redo();
    REQUIRE(staff->GetPositionCount(0) == 0);

    action.undo();
    REQUIRE(staff->GetPositionCount(0) == 1);
    REQUIRE(staff->GetPosition(0, 0)->GetNoteCount() == 1);
}

