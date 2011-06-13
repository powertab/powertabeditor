#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "../powertabdocument/score_fixture.h"
#include <actions/edittimesignature.h>

BOOST_FIXTURE_TEST_CASE(TestEditTimeSignature, ScoreFixture)
{
    TimeSignature newTimeSig(3, 4);
    newTimeSig.Show();

    const TimeSignature oldTimeSig(4, 4);

    EditTimeSignature action(&score, SystemLocation(0, 5),
                             newTimeSig);

    action.redo();
    BOOST_CHECK(system1->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    BOOST_CHECK(bar_1_5->GetTimeSignature().IsSameMeter(newTimeSig));
    BOOST_CHECK(bar_1_5->GetTimeSignature().IsShown());
    BOOST_CHECK(system2->GetStartBar()->GetTimeSignature().IsSameMeter(newTimeSig));

    BOOST_CHECK(bar_2_3->GetTimeSignature().IsSameMeter(time_2_3));
    BOOST_CHECK(system3->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    action.undo();
    BOOST_CHECK(system1->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    BOOST_CHECK(bar_1_5->GetTimeSignature().IsSameMeter(oldTimeSig));
    BOOST_CHECK(!bar_1_5->GetTimeSignature().IsShown());
    BOOST_CHECK(system2->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));

    BOOST_CHECK(bar_2_3->GetTimeSignature().IsSameMeter(time_2_3));
    BOOST_CHECK(system3->GetStartBar()->GetTimeSignature().IsSameMeter(oldTimeSig));
}
