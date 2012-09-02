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
#include <powertabdocument/powertabfileheader.h>
#include <boost/date_time/gregorian/gregorian_io.hpp>

BOOST_AUTO_TEST_SUITE(PowerTabFileHeaderTest)

    // Sanity test to ensure that conversion to/from the boost::gregorian::date structures works.
    BOOST_AUTO_TEST_CASE(BootlegDate)
    {
        boost::gregorian::date day(2012, 5, 7);
        PowerTabFileHeader header;

        header.SetSongBootlegDate(day);
        BOOST_CHECK_EQUAL(day, header.GetSongBootlegDate());
    }

BOOST_AUTO_TEST_SUITE_END()


