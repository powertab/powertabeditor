#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

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

    BOOST_AUTO_TEST_CASE(GetListOfNumbers)
    {
        AlternateEnding ending;

        std::vector<uint8_t> originalNumbers;
        originalNumbers.push_back(1);
        originalNumbers.push_back(3);
        originalNumbers.push_back(4);
        originalNumbers.push_back(7);

        BOOST_FOREACH(uint8_t number, originalNumbers)
        {
            ending.SetNumber(number);
        }

        std::vector<uint8_t> numbers = ending.GetListOfNumbers();

        BOOST_CHECK_EQUAL_COLLECTIONS(numbers.begin(), numbers.end(),
                                      originalNumbers.begin(), originalNumbers.end());
    }

BOOST_AUTO_TEST_SUITE_END()
