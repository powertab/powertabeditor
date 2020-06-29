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
  
#include <doctest/doctest.h>

#include <actions/addpositionproperty.h>
#include <score/position.h>
#include "actionfixture.h"

TEST_CASE_FIXTURE(ActionFixture, "Actions/AddPositionProperty")
{
    myLocation.getPosition()->setProperty(Position::PickStrokeUp);

    AddPositionProperty action(myLocation, Position::PickStrokeDown,
                               "Pick Stroke Up");

    // Ensure that mutually exclusive properties are cleared and reset as needed.
    action.redo();
    REQUIRE(myLocation.getPosition()->hasProperty(Position::PickStrokeDown));
    REQUIRE(!myLocation.getPosition()->hasProperty(Position::PickStrokeUp));

    action.undo();
    REQUIRE(!myLocation.getPosition()->hasProperty(Position::PickStrokeDown));
    REQUIRE(myLocation.getPosition()->hasProperty(Position::PickStrokeUp));
}
