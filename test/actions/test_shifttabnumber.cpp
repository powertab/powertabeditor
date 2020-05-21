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

#include <catch2/catch.hpp>

#include <boost/make_shared.hpp>

#include <actions/shifttabnumber.h>
#include <powertabdocument/note.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/tuning.h>

TEST_CASE("Actions/ShiftTabNumber", "")
{
    boost::shared_ptr<Staff> staff(boost::make_shared<Staff>());
    Position *pos1(new Position(0, 8, 0)), *pos2(new Position(1, 8, 0)),
             *pos3(new Position(2, 8, 0));
    staff->InsertPosition(0, pos1);
    staff->InsertPosition(0, pos2);
    staff->InsertPosition(0, pos3);

    Note *note1(new Note(2, 3)), *note2(new Note(2, 9)), *note3(new Note(2, 8));
    pos1->InsertNote(note1);
    pos2->InsertNote(note2);
    pos3->InsertNote(note3);

    note1->SetHammerOn();
    note2->SetPullOff();

    Tuning tuning;
    tuning.SetToStandard();

    ShiftTabNumber action(staff, pos2, note2, 0, Position::SHIFT_UP, tuning);

    action.redo();
    REQUIRE(!note1->HasHammerOnOrPulloff());
    REQUIRE(!note2->HasHammerOnOrPulloff());

    action.undo();
    REQUIRE(note1->HasHammerOn());
    REQUIRE(note2->HasPullOff());
}
