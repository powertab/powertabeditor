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
#include <actions/edittimesignature.h>

TEST_CASE_METHOD(ScoreFixture, "Actions/EditTimeSignature", "")
{
    TimeSignature newTimeSig(3, 4);
    newTimeSig.Show();

    const TimeSignature oldTimeSig(4, 4);

    EditTimeSignature action(&score, SystemLocation(0, 5),
                             newTimeSig);

    action.redo();
    REQUIRE(system1->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    REQUIRE(bar_1_5->GetTimeSignature().IsSameMeter(newTimeSig));
    REQUIRE(bar_1_5->GetTimeSignature().IsShown());
    REQUIRE(system2->GetStartBar()->GetTimeSignature().IsSameMeter(newTimeSig));

    REQUIRE(bar_2_3->GetTimeSignature().IsSameMeter(time_2_3));
    REQUIRE(system3->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    action.undo();
    REQUIRE(system1->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    REQUIRE(bar_1_5->GetTimeSignature().IsSameMeter(oldTimeSig));
    REQUIRE(!bar_1_5->GetTimeSignature().IsShown());
    REQUIRE(system2->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    REQUIRE(bar_2_3->GetTimeSignature().IsSameMeter(time_2_3));
    REQUIRE(system3->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));
}
