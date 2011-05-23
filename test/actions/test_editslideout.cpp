#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <actions/editslideout.h>
#include <powertabdocument/note.h>

BOOST_AUTO_TEST_CASE(TestEditSlideOut)
{
    Note note;
    uint8_t slideType = Note::slideOutOfNone;
    int8_t steps = 0;
    EditSlideOut action(&note, Note::slideOutOfLegatoSlide, 5);

    action.redo();
    note.GetSlideOutOf(slideType, steps);
    BOOST_CHECK_EQUAL(slideType, Note::slideOutOfLegatoSlide);
    BOOST_CHECK_EQUAL(steps, 5);

    action.undo();
    note.GetSlideOutOf(slideType, steps);
    BOOST_CHECK_EQUAL(slideType, Note::slideOutOfNone);
    BOOST_CHECK_EQUAL(steps, 0);

    BOOST_CHECK(action.text() == "Legato Slide");
}

