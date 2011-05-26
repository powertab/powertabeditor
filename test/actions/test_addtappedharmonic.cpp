#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <actions/addtappedharmonic.h>
#include <powertabdocument/note.h>

BOOST_AUTO_TEST_CASE(TestAddTappedHarmonic)
{
    Note note;
    const uint8_t requestedTappedFret = 17;
    uint8_t currentTappedFret = 0;

    AddTappedHarmonic action(&note, requestedTappedFret);

    action.redo();
    BOOST_CHECK(note.HasTappedHarmonic());
    note.GetTappedHarmonic(currentTappedFret);
    BOOST_CHECK_EQUAL(currentTappedFret, requestedTappedFret);

    action.undo();
    BOOST_CHECK_EQUAL(false, note.HasTappedHarmonic());
}
