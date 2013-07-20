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

#include "../powertabdocument/score_fixture.h"
#include <actions/editkeysignature.h>

TEST_CASE_METHOD(ScoreFixture, "Actions/EditKeySignature", "")
{
    EditKeySignature action(&score, SystemLocation(0, 5),
                            KeySignature::majorKey, KeySignature::fourSharps, true);

    const KeySignature newKeySig(KeySignature::majorKey, KeySignature::fourSharps);
    const KeySignature oldKeySig;

    action.redo();
    REQUIRE(system1->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    REQUIRE(bar_1_5->GetKeySignature().IsSameKey(newKeySig));
    REQUIRE(bar_1_5->GetKeySignature().IsShown());
    REQUIRE(system2->GetStartBar()->GetKeySignature().IsSameKey(newKeySig));

    REQUIRE(bar_2_3->GetKeySignature().IsSameKey(key_2_3));
    REQUIRE(system3->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    action.undo();
    REQUIRE(system1->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    REQUIRE(bar_1_5->GetKeySignature().IsSameKey(oldKeySig));
    REQUIRE(!bar_1_5->GetKeySignature().IsShown());
    REQUIRE(system2->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    REQUIRE(bar_2_3->GetKeySignature().IsSameKey(key_2_3));
    REQUIRE(system3->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));
}

