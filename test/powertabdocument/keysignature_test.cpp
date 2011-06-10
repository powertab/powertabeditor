#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/keysignature.h>

BOOST_AUTO_TEST_SUITE(KeySignatureTest)

    BOOST_AUTO_TEST_CASE(GetText)
    {
        KeySignature key(KeySignature::majorKey, KeySignature::fourFlats);

        BOOST_CHECK_EQUAL(key.GetText(), "Ab Major - Bb Eb Ab Db");

        key.SetKey(KeySignature::majorKey, KeySignature::noAccidentals);
        BOOST_CHECK_EQUAL(key.GetText(), "C Major");

        key.SetKey(KeySignature::minorKey, KeySignature::twoSharps);
        BOOST_CHECK_EQUAL(key.GetText(), "B Minor - F# C#");
    }

BOOST_AUTO_TEST_SUITE_END()


