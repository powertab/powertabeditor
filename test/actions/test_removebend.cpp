/*
  * Copyright (C) 2014 Cameron White
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

#include <catch2/catch.hpp>

#include <actions/removespecialnoteproperty.h>
#include <score/note.h>
#include "actionfixture.h"

TEST_CASE_METHOD(ActionFixture, "Actions/RemoveBend")
{
	Bend bend(Bend::BendAndHold, 3, 2, 5, Bend::MidPoint, Bend::HighPoint);
    myLocation.getNote()->setBend(bend);

    RemoveBend action(myLocation);

    action.redo();
    REQUIRE(!myLocation.getNote()->hasBend());

    action.undo();
    REQUIRE(myLocation.getNote()->hasBend());
    REQUIRE(myLocation.getNote()->getBend() == bend);
}

