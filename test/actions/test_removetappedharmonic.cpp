#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <actions/removetappedharmonic.h>
#include <powertabdocument/note.h>

BOOST_AUTO_TEST_CASE(TestRemoveTappedHarmonic)
{
    Note note;
    const uint8_t originalTappedFret = 17;
    note.SetTappedHarmonic(originalTappedFret);
    uint8_t currentTappedFret = 0;

    RemoveTappedHarmonic action(&note);

    action.redo();
    BOOST_CHECK_EQUAL(false, note.HasTappedHarmonic());

    action.undo();
    BOOST_CHECK(note.HasTappedHarmonic());
    note.GetTappedHarmonic(currentTappedFret);
    BOOST_CHECK_EQUAL(currentTappedFret, originalTappedFret);
}

