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

#include <powertabdocument/system.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/barline.h>

BOOST_AUTO_TEST_SUITE(SystemTest)

    BOOST_AUTO_TEST_SUITE(FindStaffIndex)

        BOOST_AUTO_TEST_CASE(noStaves)
        {
            System emptySystem;
            boost::shared_ptr<Staff> staff;

            BOOST_CHECK_THROW(emptySystem.FindStaffIndex(staff), std::out_of_range);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_CASE(CopyAndEquality)
    {
        System system1;
        system1.InsertBarline(System::BarlinePtr(new Barline(2, Barline::doubleBar, 0)));
        System system2 = system1;

        BOOST_CHECK(system1 == system2);

        // check deep copy
        system1.GetBarlineAtPosition(2)->SetBarlineData(Barline::repeatStart, 0);
        BOOST_CHECK(system1 != system2);
    }

BOOST_AUTO_TEST_SUITE_END()
