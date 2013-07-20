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

#include <catch.hpp>

#include <actions/editrest.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

TEST_CASE("Actions/EditRest/NoExistingRest", "")
{
    const System::StaffPtr staff(new Staff());
    const uint32_t voice = 0;
    const uint8_t duration = 16;

    Position* pos = NULL;
    REQUIRE(staff->GetPositionCount(voice) == 0);

    EditRest action(pos, staff, 0, voice, duration);

    action.redo();

    REQUIRE(staff->GetPositionCount(voice) == 1);

    action.undo();

    REQUIRE(staff->GetPositionCount(voice) == 0);
}

TEST_CASE("Actions/EditRest/ModifyExistingRest", "")
{
    const System::StaffPtr staff(new Staff());
    const uint32_t voice = 0;
    const uint8_t newDuration = 16;
    const uint8_t oldDuration = 4;

    Position *pos = new Position(0, oldDuration, 0);
    pos->SetRest(true);
    staff->InsertPosition(voice, pos);
    REQUIRE(staff->GetPositionCount(voice) == 1);

    EditRest action(pos, staff, staff->GetIndexOfPosition(voice, pos), voice, newDuration);

    action.redo();

    REQUIRE(staff->GetPositionCount(voice) == 1);
    REQUIRE(pos->IsRest());
    REQUIRE(pos->GetDurationType() == newDuration);

    action.undo();

    REQUIRE(staff->GetPositionCount(voice) == 1);
    REQUIRE(pos->IsRest());
    REQUIRE(pos->GetDurationType() == oldDuration);
}

TEST_CASE("Actions/EditRest/RemoveRest", "")
{
    const System::StaffPtr staff(new Staff());
    const uint32_t voice = 0;
    const uint8_t oldDuration = 4;

    Position* pos = new Position(0, oldDuration, 0);
    pos->SetRest(true);
    staff->InsertPosition(voice, pos);
    REQUIRE(staff->GetPositionCount(voice) == 1);

    EditRest action(pos, staff, staff->GetIndexOfPosition(voice, pos), voice, oldDuration);

    action.redo();

    REQUIRE(staff->GetPositionCount(voice) == 0);
    REQUIRE(pos->IsRest() == false);

    action.undo();

    REQUIRE(staff->GetPositionCount(voice) == 1);
    REQUIRE(pos->IsRest());
    REQUIRE(pos->GetDurationType() == 4);
}
