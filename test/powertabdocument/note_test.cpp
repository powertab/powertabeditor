#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "serialization_test.h"
#include <powertabdocument/note.h>

BOOST_AUTO_TEST_SUITE(NoteTest)

    BOOST_AUTO_TEST_CASE(Serialization)
    {
        Note note(4, 5);
        testSerialization(note);
    }

BOOST_AUTO_TEST_SUITE_END()

