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
    Position pos;

    EditRest action(&pos, 16);

    action.redo();

    REQUIRE(pos.IsRest());
    REQUIRE(pos.GetDurationType() == 16);

    action.undo();

    REQUIRE(!pos.IsRest());
    REQUIRE(pos.GetNoteCount() == 0u);
}

TEST_CASE("Actions/EditRest/ModifyExistingRest", "")
{
    Position pos;
    pos.SetRest(true);
    pos.SetDurationType(4);

    EditRest action(&pos, 16);

    action.redo();

    REQUIRE(pos.IsRest());
    REQUIRE(pos.GetDurationType() == 16);

    action.undo();

    REQUIRE(pos.IsRest());
    REQUIRE(pos.GetDurationType() == 4);
}

TEST_CASE("Actions/EditRest/RemoveRest", "")
{
    Position pos;
    pos.SetRest(true);
    pos.SetDurationType(4);

    EditRest action(&pos, 4);

    action.redo();

    REQUIRE(pos.IsRest() == false);

    action.undo();

    REQUIRE(pos.IsRest());
    REQUIRE(pos.GetDurationType() == 4);
}
