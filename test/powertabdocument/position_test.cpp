#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

struct PositionFixture
{
    PositionFixture()
    {
        note1 = new Note(1, 0);
        note2 = new Note(3, 0);
        note3 = new Note(5, 0);
        pos.m_noteArray.push_back(note1);
        pos.m_noteArray.push_back(note2);
        pos.m_noteArray.push_back(note3);
    }

    ~PositionFixture() {}

    Note *note1, *note2, *note3;
    Position pos;
};

BOOST_AUTO_TEST_SUITE(PositionTest)

    BOOST_FIXTURE_TEST_CASE(GetNoteByString, PositionFixture)
    {
        BOOST_CHECK_EQUAL(pos.GetNoteByString(2), (Note*)NULL);
        BOOST_CHECK_EQUAL(pos.GetNoteByString(1), note1);
        BOOST_CHECK_EQUAL(pos.GetNoteByString(3), note2);
    }

    BOOST_AUTO_TEST_SUITE(GetStringBounds)

        BOOST_FIXTURE_TEST_CASE(MultipleStrings, PositionFixture)
        {
            BOOST_CHECK(pos.GetStringBounds() == (std::pair<uint8_t, uint8_t>(1, 5)));
        }

        BOOST_AUTO_TEST_CASE(SingleString)
        {
            Position pos;
            pos.m_noteArray.push_back(new Note(4, 13));

            BOOST_CHECK(pos.GetStringBounds() == (std::pair<uint8_t, uint8_t>(4, 4)));
        }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
