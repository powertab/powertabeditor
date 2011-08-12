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
  
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "../powertabdocument/score_fixture.h"
#include <actions/editkeysignature.h>

BOOST_FIXTURE_TEST_CASE(TestEditKeySignature, ScoreFixture)
{
    EditKeySignature action(&score, SystemLocation(0, 5),
                            KeySignature::majorKey, KeySignature::fourSharps, true);

    const KeySignature newKeySig(KeySignature::majorKey, KeySignature::fourSharps);
    const KeySignature oldKeySig;

    action.redo();
    BOOST_CHECK(system1->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    BOOST_CHECK(bar_1_5->GetKeySignature().IsSameKey(newKeySig));
    BOOST_CHECK(bar_1_5->GetKeySignature().IsShown());
    BOOST_CHECK(system2->GetStartBar()->GetKeySignature().IsSameKey(newKeySig));

    BOOST_CHECK(bar_2_3->GetKeySignature().IsSameKey(key_2_3));
    BOOST_CHECK(system3->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    action.undo();
    BOOST_CHECK(system1->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    BOOST_CHECK(bar_1_5->GetKeySignature().IsSameKey(oldKeySig));
    BOOST_CHECK(!bar_1_5->GetKeySignature().IsShown());
    BOOST_CHECK(system2->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    BOOST_CHECK(bar_2_3->GetKeySignature().IsSameKey(key_2_3));
    BOOST_CHECK(system3->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));
}

