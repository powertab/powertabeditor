#ifndef SCORE_FIXTURE_H
#define SCORE_FIXTURE_H

#include "../powertabdocument/tuning_fixtures.h"
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/keysignature.h>

/// Provides a simple Score setup - barline at location (0,5), and another
/// barline at (1,3), with a different key signature and time signature
struct ScoreFixture
{
    ScoreFixture() :
        system1(new System),
        system2(new System),
        system3(new System),
        bar_1_5(new Barline(5, Barline::doubleBar, 2)),
        bar_2_3(new Barline(3, Barline::doubleBar, 2)),
        key_2_3(KeySignature::majorKey, KeySignature::fourFlats),
        time_2_3(5, 4)
    {
        Score::GuitarPtr guitar(new Guitar);
        guitar->SetTuning(StandardTuningFixture().tuning);

        score.InsertGuitar(guitar);
        score.InsertSystem(system1, 0);
        score.InsertSystem(system2, 1);
        score.InsertSystem(system3, 2);

        system1->InsertBarline(bar_1_5);

        bar_2_3->SetKeySignature(key_2_3);
        bar_2_3->SetTimeSignature(time_2_3);
        system2->InsertBarline(bar_2_3);
    }

    Score score;
    Score::SystemPtr system1, system2, system3;
    System::BarlinePtr bar_1_5, bar_2_3;
    KeySignature key_2_3;
    TimeSignature time_2_3;
};

#endif // SCORE_FIXTURE_H
