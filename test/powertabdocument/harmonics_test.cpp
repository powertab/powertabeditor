#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <powertabdocument/harmonics.h>

BOOST_AUTO_TEST_SUITE(HarmonicsTest)

    BOOST_AUTO_TEST_CASE(PitchOffset)
    {
        // no harmonic occurs at this fret
        BOOST_CHECK_EQUAL(Harmonics::getPitchOffset(1), 0);

        // some typical natural harmonics
        BOOST_CHECK_EQUAL(Harmonics::getPitchOffset(12), 12);
        BOOST_CHECK_EQUAL(Harmonics::getPitchOffset(7), 19);
    }

    BOOST_AUTO_TEST_CASE(FretOffsets)
    {
        std::vector<uint8_t> frets = Harmonics::getFretOffsets();

        // just verify that a few of the typical harmonics are there
        BOOST_CHECK(!frets.empty());
        BOOST_CHECK(std::find(frets.begin(), frets.end(), 12) != frets.end());
    }

BOOST_AUTO_TEST_SUITE_END()

