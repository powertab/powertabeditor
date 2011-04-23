#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/alternateending.h>

BOOST_AUTO_TEST_SUITE(AlternateEndingTest)

    BOOST_AUTO_TEST_CASE(GetText)
    {
        AlternateEnding ending;

        // empty
        BOOST_CHECK_EQUAL(ending.GetText(), "");

        ending.SetNumber(1);
        BOOST_CHECK_EQUAL(ending.GetText(), "1.");

        ending.SetNumber(2);
        BOOST_CHECK_EQUAL(ending.GetText(), "1., 2.");

        ending.SetNumber(3);
        BOOST_CHECK_EQUAL(ending.GetText(), "1.-3.");

        ending.SetNumbers(1 | 16 | 32 | 64 | 512);
        BOOST_CHECK_EQUAL(ending.GetText(), "1., 5.-7., D.S.");
    }

BOOST_AUTO_TEST_SUITE_END()
