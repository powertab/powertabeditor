#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <actions/editslideinto.h>
#include <powertabdocument/note.h>

BOOST_AUTO_TEST_CASE(TestEditSlideInto)
{
    Note note;
    uint8_t slideType = 0;
    EditSlideInto action(&note, Note::slideIntoFromBelow);

    action.redo();
    note.GetSlideInto(slideType);
    BOOST_CHECK_EQUAL(slideType, Note::slideIntoFromBelow);

    action.undo();
    note.GetSlideInto(slideType);
    BOOST_CHECK_EQUAL(slideType, Note::slideIntoNone);
}
