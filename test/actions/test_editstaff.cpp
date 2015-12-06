/*
  * Copyright (C) 2015 Cameron White
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

#include <actions/editstaff.h>
#include <app/appinfo.h>
#include <formats/powertab/powertabimporter.h>
#include <score/score.h>

TEST_CASE("Actions/EditStaff", "")
{
    Score score;

    PowerTabImporter importer;
    importer.load(AppInfo::getAbsolutePath("data/test_editstaff.pt2"), score);

    ScoreLocation location(score, 0, 0);
    EditStaff action(location, Staff::TrebleClef, 5);

    action.redo();

    const System &system = score.getSystems()[0];
    const System &next_system = score.getSystems()[1];
    const Staff &staff = system.getStaves()[0];
    REQUIRE(staff.getStringCount() == 5);
    REQUIRE(staff.getVoices()[0].getPositions().size() == 5);
    REQUIRE(system.getPlayerChanges().size() == 2);
    REQUIRE(system.getPlayerChanges()[0].getActivePlayers(0).empty());
    REQUIRE(system.getPlayerChanges()[1].getActivePlayers(0).empty());
    REQUIRE(next_system.getPlayerChanges().size() == 1);
    REQUIRE(next_system.getPlayerChanges()[0].getPosition() == 0);

    action.undo();
    REQUIRE(staff.getStringCount() == 6);
    REQUIRE(staff.getVoices()[0].getPositions().size() == 6);
    REQUIRE(system.getPlayerChanges()[0].getActivePlayers(0).size() == 1);
    REQUIRE(system.getPlayerChanges()[1].getActivePlayers(0).size() == 1);
    REQUIRE(next_system.getPlayerChanges().empty());
}
