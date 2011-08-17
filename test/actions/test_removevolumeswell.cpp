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

#include <actions/removevolumeswell.h>
#include <powertabdocument/position.h>
#include <powertabdocument/dynamic.h>

BOOST_AUTO_TEST_CASE(TestRemoveVolumeSwell)
{
    Position pos;
    pos.SetVolumeSwell(Dynamic::mf, Dynamic::ff, 3);

    RemoveVolumeSwell action(&pos);

    action.redo();
    BOOST_CHECK(!pos.HasVolumeSwell());

    action.undo();
    BOOST_CHECK(pos.HasVolumeSwell());

    uint8_t startVol, endVol, duration;
    pos.GetVolumeSwell(startVol, endVol, duration);

    BOOST_CHECK_EQUAL(startVol, Dynamic::mf);
    BOOST_CHECK_EQUAL(endVol, Dynamic::ff);
    BOOST_CHECK_EQUAL(duration, 3u);
}
