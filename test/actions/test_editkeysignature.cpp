#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "../powertabdocument/tuning_fixtures.h"
#include <actions/editkeysignature.h>
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/keysignature.h>

struct ScoreFixture
{
    ScoreFixture() :
        system1(new System),
        system2(new System),
        system3(new System),
        bar_1_5(new Barline(5, Barline::doubleBar, 2)),
        bar_2_3(new Barline(3, Barline::doubleBar, 2)),
        key_2_3(KeySignature::majorKey, KeySignature::fourFlats)
    {
        Score::GuitarPtr guitar(new Guitar);
        guitar->SetTuning(StandardTuningFixture().tuning);

        score.InsertGuitar(guitar);
        score.InsertSystem(system1, 0);
        score.InsertSystem(system2, 1);
        score.InsertSystem(system3, 2);

        system1->InsertBarline(bar_1_5);

        bar_2_3->SetKeySignature(key_2_3);
        system2->InsertBarline(bar_2_3);
    }

    Score score;
    Score::SystemPtr system1, system2, system3;
    System::BarlinePtr bar_1_5, bar_2_3;
    KeySignature key_2_3;
};

BOOST_FIXTURE_TEST_CASE(TestEditKeySignature, ScoreFixture)
{
    EditKeySignature action(&score, SystemLocation(0, 5),
                            KeySignature::majorKey, KeySignature::fourSharps);

    const KeySignature newKeySig(KeySignature::majorKey, KeySignature::fourSharps);
    const KeySignature oldKeySig;

    action.redo();
    BOOST_CHECK(system1->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    BOOST_CHECK(bar_1_5->GetKeySignature().IsSameKey(newKeySig));
    BOOST_CHECK(system2->GetStartBar()->GetKeySignature().IsSameKey(newKeySig));

    BOOST_CHECK(bar_2_3->GetKeySignature().IsSameKey(key_2_3));
    BOOST_CHECK(system3->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    action.undo();
    BOOST_CHECK(system1->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    BOOST_CHECK(bar_1_5->GetKeySignature().IsSameKey(oldKeySig));
    BOOST_CHECK(system2->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));

    BOOST_CHECK(bar_2_3->GetKeySignature().IsSameKey(key_2_3));
    BOOST_CHECK(system3->GetStartBar()->GetKeySignature().IsSameKey(oldKeySig));
}

