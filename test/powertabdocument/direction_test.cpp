#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "serialization_test.h"
#include <powertabdocument/direction.h>

BOOST_AUTO_TEST_SUITE(DirectionTest)

    BOOST_AUTO_TEST_CASE(Serialization)
    {
        Direction direction(4, Direction::daCapoAlDoubleCoda, Direction::activeDalSegno, 2);
        testSerialization(direction);
    }

BOOST_AUTO_TEST_SUITE_END()

