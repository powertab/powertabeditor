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

#include <boost/make_shared.hpp>
#include <actions/addmusicaldirection.h>
#include <powertabdocument/direction.h>
#include <powertabdocument/system.h>

TEST_CASE("Actions/AddMusicalDirection", "")
{
    boost::shared_ptr<System> system(boost::make_shared<System>());
    boost::shared_ptr<Direction> dir(boost::make_shared<Direction>());

    AddMusicalDirection action(system, dir);

    REQUIRE(system->GetDirectionCount() == 0);

    action.redo();
    REQUIRE(system->GetDirectionCount() == 1);
    REQUIRE(system->GetDirection(0) == dir);

    action.undo();
    REQUIRE(system->GetDirectionCount() == 0);
}

