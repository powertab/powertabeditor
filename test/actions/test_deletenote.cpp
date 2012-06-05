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

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <actions/deletenote.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

BOOST_AUTO_TEST_CASE(TestCanDeleteNote)
{
    BOOST_CHECK(!DeleteNote::canExecute(NULL, 2));

    Position position;
    BOOST_CHECK(!DeleteNote::canExecute(&position, 3));

    position.InsertNote(new Note(3, 12));
    BOOST_CHECK(DeleteNote::canExecute(&position, 3));
    BOOST_CHECK(!DeleteNote::canExecute(&position, 2));
}

BOOST_AUTO_TEST_CASE(TestDeleteNote)
{
    const uint32_t string1 = 2, string2 = 4;
    const uint8_t fret1 = 12, fret2 = 13;

    boost::shared_ptr<Staff> staff(new Staff);
    Position* position = new Position;
    position->InsertNote(new Note(string1, fret1));
    position->InsertNote(new Note(string2, fret2));
    staff->InsertPosition(0, position);

    DeleteNote action(staff, /* voice */ 0, position, string1);

    action.redo();
    BOOST_CHECK_EQUAL(position->GetNoteCount(), 1);
    BOOST_CHECK(!position->GetNoteByString(string1));
    BOOST_CHECK(position->GetNoteByString(string2));

    action.undo();
    BOOST_CHECK_EQUAL(position->GetNoteCount(), 2);
    BOOST_CHECK(position->GetNoteByString(string1));
    BOOST_CHECK(position->GetNoteByString(string2));
    BOOST_CHECK_EQUAL(position->GetNoteByString(string1)->GetFretNumber(), fret1);
    BOOST_CHECK_EQUAL(position->GetNoteByString(string2)->GetFretNumber(), fret2);
}

BOOST_AUTO_TEST_CASE(TestDeleteLastNote)
{
    boost::shared_ptr<Staff> staff(new Staff);
    Position* position = new Position;
    position->InsertNote(new Note(2, 12));
    staff->InsertPosition(0, position);

    DeleteNote action(staff, /* voice */ 0, position, 2);

    action.redo();
    BOOST_CHECK_EQUAL(staff->GetPositionCount(0), 0);

    action.undo();
    BOOST_CHECK_EQUAL(staff->GetPositionCount(0), 1);
    BOOST_CHECK_EQUAL(staff->GetPosition(0, 0)->GetNoteCount(), 1);
}

