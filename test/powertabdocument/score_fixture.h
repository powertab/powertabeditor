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
