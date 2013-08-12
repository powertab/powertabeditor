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
#include <score/staffutils.h>
#include "test_serialization.h"

TEST_CASE("Score/Staff/Clef", "")
{
    Staff staff;

    REQUIRE(staff.getClefType() == Staff::TrebleClef);
    staff.setClefType(Staff::BassClef);
    REQUIRE(staff.getClefType() == Staff::BassClef);
}

TEST_CASE("Score/Staff/Positions", "")
{
    Staff staff;

    REQUIRE(staff.getVoice(0).size() == 0);
    REQUIRE(staff.getVoice(1).size() == 0);
    REQUIRE_THROWS(staff.getVoice(2));

    Position pos1(3), pos2(5), pos3(1);

    staff.insertPosition(1, pos1);
    staff.insertPosition(1, pos2);
    staff.insertPosition(1, pos3);

    REQUIRE(staff.getVoice(1).size() == 3);
    staff.removePosition(1, pos1);

    REQUIRE(staff.getVoice(1).size() == 2);
    REQUIRE(staff.getVoice(1)[0] == pos3);
    REQUIRE(staff.getVoice(1)[1] == pos2);
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
    staff.insertPosition(1, Position(42));
    staff.insertDynamic(Dynamic(11, Dynamic::pp));
    staff.setStringCount(7);

    Serialization::test(staff);
}

TEST_CASE("Score/Staff/GetPositionsInRange", "")
{
    Staff staff;
    Position pos1(1), pos4(4), pos6(6), pos7(7), pos8(8);
    staff.insertPosition(0, pos1);
    staff.insertPosition(0, pos4);
    staff.insertPosition(0, pos6);
    staff.insertPosition(0, pos7);
    staff.insertPosition(0, pos8);

    REQUIRE(std::distance(StaffUtils::getPositionsInRange(staff, 0, 9, 15).begin(),
                          StaffUtils::getPositionsInRange(staff, 0, 9, 15).end()) == 0);
    REQUIRE(std::distance(StaffUtils::getPositionsInRange(staff, 0, 8, 10).begin(),
                          StaffUtils::getPositionsInRange(staff, 0, 8, 10).end()) == 1);
    REQUIRE(std::distance(StaffUtils::getPositionsInRange(staff, 0, 4, 7).begin(),
                          StaffUtils::getPositionsInRange(staff, 0, 4, 7).end()) == 3);
}
