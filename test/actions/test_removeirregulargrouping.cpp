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
#include <boost/foreach.hpp>

#include <actions/removeirregulargrouping.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

BOOST_AUTO_TEST_CASE(TestRemoveIrregularGrouping)
{
    boost::shared_ptr<Staff> staff(new Staff);

    staff->InsertPosition(0, new Position(0, 1, 0));
    staff->InsertPosition(0, new Position(1, 1, 0));
    staff->InsertPosition(0, new Position(2, 1, 0));
    staff->InsertPosition(0, new Position(3, 1, 0));
    staff->InsertPosition(0, new Position(4, 1, 0));
    staff->InsertPosition(0, new Position(5, 1, 0));

    for (size_t i = 1; i <= 4; i++)
    {
        staff->GetPosition(0, i)->SetIrregularGroupingTiming(3, 2);
    }

    staff->GetPosition(0, 1)->SetIrregularGroupingStart();
    staff->GetPosition(0, 2)->SetIrregularGroupingMiddle();
    staff->GetPosition(0, 3)->SetIrregularGroupingMiddle();
    staff->GetPosition(0, 4)->SetIrregularGroupingEnd();

    RemoveIrregularGrouping action(staff, staff->GetPosition(0, 3));

    action.redo();

    for (size_t i = 0; i < staff->GetPositionCount(0); i++)
    {
        BOOST_CHECK(!staff->GetPosition(0, i)->HasIrregularGroupingTiming());
        BOOST_CHECK(!staff->GetPosition(0, i)->IsIrregularGroupingStart());
        BOOST_CHECK(!staff->GetPosition(0, i)->IsIrregularGroupingMiddle());
        BOOST_CHECK(!staff->GetPosition(0, i)->IsIrregularGroupingEnd());
    }

    action.undo();

    for (size_t i = 1; i <= 4; i++)
    {
        BOOST_CHECK(staff->GetPosition(0, i)->HasIrregularGroupingTiming());
    }

    BOOST_CHECK(staff->GetPosition(0, 1)->IsIrregularGroupingStart());
    BOOST_CHECK(staff->GetPosition(0, 2)->IsIrregularGroupingMiddle());
    BOOST_CHECK(staff->GetPosition(0, 3)->IsIrregularGroupingMiddle());
    BOOST_CHECK(staff->GetPosition(0, 4)->IsIrregularGroupingEnd());
}
