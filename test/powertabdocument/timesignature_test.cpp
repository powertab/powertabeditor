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

#include <powertabdocument/timesignature.h>

BOOST_AUTO_TEST_SUITE(TimeSignatureTests)

    BOOST_AUTO_TEST_CASE(ValidPulses)
    {
        TimeSignature time;

        time.SetBeatsPerMeasure(6);

        BOOST_CHECK(time.IsValidPulses(0));
        BOOST_CHECK(time.IsValidPulses(1));
        BOOST_CHECK(time.IsValidPulses(2));
        BOOST_CHECK(time.IsValidPulses(3));
        BOOST_CHECK(time.IsValidPulses(6));

        BOOST_CHECK(!time.IsValidPulses(4));
        BOOST_CHECK(!time.IsValidPulses(7));
        BOOST_CHECK(!time.IsValidPulses(12));
    }

BOOST_AUTO_TEST_SUITE_END()
