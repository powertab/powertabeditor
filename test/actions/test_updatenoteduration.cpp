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

#include <actions/updatenoteduration.h>
#include <powertabdocument/position.h>

BOOST_AUTO_TEST_CASE(TestUpdateNoteDuration)
{
    Position pos1(0, 4, 0), pos2(1, 2, 0), pos3(2, 1, 0);

    std::vector<Position*> positions;
    positions.push_back(&pos1);
    positions.push_back(&pos2);
    positions.push_back(&pos3);

    UpdateNoteDuration action(positions, 8);

    action.redo();
    BOOST_CHECK_EQUAL(pos1.GetDurationType(), 8);
    BOOST_CHECK_EQUAL(pos2.GetDurationType(), 8);
    BOOST_CHECK_EQUAL(pos3.GetDurationType(), 8);

    action.undo();
    BOOST_CHECK_EQUAL(pos1.GetDurationType(), 4);
    BOOST_CHECK_EQUAL(pos2.GetDurationType(), 2);
    BOOST_CHECK_EQUAL(pos3.GetDurationType(), 1);
}
