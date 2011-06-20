#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/timesignature.h>

BOOST_AUTO_TEST_SUITE(TimeSignatureTests)

    BOOST_AUTO_TEST_CASE(ValidPulses)
    {
        TimeSignature time;

        time.SetBeatsPerMeasure(6);

        BOOST_CHECK(time.IsValidPulses(0));
        BOOST_CHECK(time.IsValidPulses(1));
        BOOST_CHECK(time.IsValidPulses(2));
        BOOST_CHECK(time.IsValidPulses(3));
        BOOST_CHECK(time.IsValidPulses(6));

        BOOST_CHECK(!time.IsValidPulses(4));
        BOOST_CHECK(!time.IsValidPulses(7));
        BOOST_CHECK(!time.IsValidPulses(12));
    }

BOOST_AUTO_TEST_SUITE_END()
