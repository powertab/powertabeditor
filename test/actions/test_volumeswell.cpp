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

#include <doctest/doctest.h>

#include "actionfixture.h"
#include <actions/volumeswell.h>
#include <score/position.h>

TEST_CASE_FIXTURE(ActionFixture, "Actions/AddVolumeSwell")
{
    const VolumeSwell swell(VolumeLevel::mf, VolumeLevel::ff, 3);
    AddVolumeSwell action(myLocation, swell);

    action.redo();
    REQUIRE(myLocation.getPosition()->hasVolumeSwell());
    REQUIRE(myLocation.getPosition()->getVolumeSwell() == swell);

    REQUIRE(swell.getStartVolume() == VolumeLevel::mf);
    REQUIRE(swell.getEndVolume() == VolumeLevel::ff);
    REQUIRE(swell.getDuration() == 3);

    action.undo();
    REQUIRE(!myLocation.getPosition()->hasVolumeSwell());
}

TEST_CASE_FIXTURE(ActionFixture, "Actions/RemoveVolumeSwell")
{
    const VolumeSwell swell(VolumeLevel::mf, VolumeLevel::ff, 3);
    myLocation.getPosition()->setVolumeSwell(swell);

    RemoveVolumeSwell action(myLocation);

    action.redo();
    REQUIRE(!myLocation.getPosition()->hasVolumeSwell());

    action.undo();
    REQUIRE(myLocation.getPosition()->hasVolumeSwell());
    REQUIRE(myLocation.getPosition()->getVolumeSwell() == swell);
}
