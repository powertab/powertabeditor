#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/score.h>
#include <powertabdocument/alternateending.h>
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

BOOST_AUTO_TEST_SUITE_END()

