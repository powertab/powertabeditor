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
  
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <actions/editrest.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

BOOST_AUTO_TEST_SUITE(TestEditRest)

    BOOST_AUTO_TEST_CASE(NoExistingRest)
    {
        Position pos;

        EditRest action(&pos, 16);

        action.redo();

        BOOST_CHECK(pos.IsRest());
        BOOST_CHECK_EQUAL(pos.GetDurationType(), 16);

        action.undo();

        BOOST_CHECK(!pos.IsRest());
        BOOST_CHECK_EQUAL(pos.GetNoteCount(), 0u);
    }

    BOOST_AUTO_TEST_CASE(ModifyExistingRest)
    {
        Position pos;
        pos.SetRest(true);
        pos.SetDurationType(4);

        EditRest action(&pos, 16);

        action.redo();

        BOOST_CHECK(pos.IsRest());
        BOOST_CHECK_EQUAL(pos.GetDurationType(), 16);

        action.undo();

        BOOST_CHECK(pos.IsRest());
        BOOST_CHECK_EQUAL(pos.GetDurationType(), 4);
    }

    BOOST_AUTO_TEST_CASE(RemoveRest)
    {
        Position pos;
        pos.SetRest(true);
        pos.SetDurationType(4);

        EditRest action(&pos, 4);

        action.redo();

        BOOST_CHECK(pos.IsRest() == false);

        action.undo();

        BOOST_CHECK(pos.IsRest());
        BOOST_CHECK_EQUAL(pos.GetDurationType(), 4);
    }

BOOST_AUTO_TEST_SUITE_END()
