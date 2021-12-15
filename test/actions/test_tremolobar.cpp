/*
 * Copyright (C) 2021 Cameron White
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

#include "actionfixture.h"
#include <actions/tremolobar.h>
#include <score/position.h>

TEST_CASE_FIXTURE(ActionFixture, "Actions/AddTremoloBar")
{
    const TremoloBar trem(TremoloBar::Type::InvertedDip, 3, 5);
    AddTremoloBar action(myLocation, trem);

    action.redo();
    REQUIRE(myLocation.getPosition()->hasTremoloBar());
    REQUIRE(myLocation.getPosition()->getTremoloBar() == trem);

    REQUIRE(trem.getType() == TremoloBar::Type::InvertedDip);
    REQUIRE(trem.getPitch() == 3);
    REQUIRE(trem.getDuration() == 5);

    action.undo();
    REQUIRE(!myLocation.getPosition()->hasTremoloBar());
}

TEST_CASE_FIXTURE(ActionFixture, "Actions/RemoveTremoloBar")
{
    const TremoloBar trem(TremoloBar::Type::InvertedDip, 3, 5);
    myLocation.getPosition()->setTremoloBar(trem);

    RemoveTremoloBar action(myLocation);

    action.redo();
    REQUIRE(!myLocation.getPosition()->hasTremoloBar());

    action.undo();
    REQUIRE(myLocation.getPosition()->hasTremoloBar());
    REQUIRE(myLocation.getPosition()->getTremoloBar() == trem);
}
