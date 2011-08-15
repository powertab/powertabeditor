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
#include <boost/make_shared.hpp>

#include <powertabdocument/score.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/dynamic.h>
#include <powertabdocument/tempomarker.h>

BOOST_AUTO_TEST_SUITE(ScoreTest)

    BOOST_AUTO_TEST_CASE(CopyAndEquality)
    {
        Score score1, score2;
        
        // basic checks for equality
        BOOST_CHECK(score1 == score2);
        score1.InsertAlternateEnding(Score::AlternateEndingPtr(new AlternateEnding));
        BOOST_CHECK(score1 != score2);
        
        // check for deep copy and equality
        Score score3(score1);
        BOOST_CHECK(score3 == score1);
        score3.GetAlternateEnding(0)->SetDalSegno();
        BOOST_CHECK(*score3.GetAlternateEnding(0) != *score1.GetAlternateEnding(0));
    }

    BOOST_AUTO_TEST_CASE(InsertTempoMarker)
    {
        Score score;

        BOOST_CHECK_EQUAL(score.GetTempoMarkerCount(), 0u);

        score.InsertTempoMarker(Score::TempoMarkerPtr(new TempoMarker(0, 0, false)));

        BOOST_CHECK_EQUAL(score.GetTempoMarkerCount(), 1u);
    }

    BOOST_AUTO_TEST_CASE(Dynamics)
    {
        Score score;
        BOOST_CHECK_EQUAL(score.GetDynamicCount(), 0u);

        boost::shared_ptr<Dynamic> dynamic = boost::make_shared<Dynamic>(1, 2, 3, Dynamic::mp, Dynamic::notSet);
        score.InsertDynamic(dynamic);

        BOOST_CHECK_EQUAL(score.GetDynamicCount(), 1u);
        BOOST_CHECK_EQUAL(score.FindDynamic(1, 2, 3), dynamic);

        score.RemoveDynamic(dynamic);
        BOOST_CHECK_EQUAL(score.GetDynamicCount(), 0u);
    }

BOOST_AUTO_TEST_SUITE_END()

