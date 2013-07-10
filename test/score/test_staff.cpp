/*
  * Copyright (C) 2013 Cameron White
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

#include <score/staff.h>
#include "test_serialization.h"

using namespace Score; 

TEST_CASE("Score/Staff/Clef", "")
{
    Staff staff;

    REQUIRE(staff.getClefType() == Staff::TrebleClef);
    staff.setClefType(Staff::BassClef);
    REQUIRE(staff.getClefType() == Staff::BassClef);
}

TEST_CASE("Score/Staff/Dynamics", "")
{
    Staff staff;

    REQUIRE(staff.getDynamics().size() == 0);

    Dynamic dynamic(3, Dynamic::mf);
    staff.insertDynamic(dynamic);
    staff.insertDynamic(Dynamic(1, Dynamic::pp));
    REQUIRE(staff.getDynamics().size() == 2);
    REQUIRE(staff.getDynamics()[1] == dynamic);

    staff.removeDynamic(dynamic);
    REQUIRE(staff.getDynamics().size() == 1);
}

TEST_CASE("Score/Staff/Serialization", "")
{
    Staff staff;
    staff.setViewType(Staff::BassView);
    staff.setClefType(Staff::BassClef);
    staff.insertDynamic(Dynamic(11, Dynamic::pp));
    staff.setStringCount(7);

    Serialization::test(staff);
}
