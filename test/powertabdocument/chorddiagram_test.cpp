#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "serialization_test.h"
#include <powertabdocument/chorddiagram.h>

BOOST_AUTO_TEST_SUITE(ChordDiagramTest)

    BOOST_AUTO_TEST_CASE(Serialization)
    {
        ChordDiagram diagram(7, 1, 2, 3, 4);
        testSerialization(diagram);
    }

BOOST_AUTO_TEST_SUITE_END()
