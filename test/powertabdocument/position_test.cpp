#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

struct PositionFixture
{
    PositionFixture()
    {
        pos = new Position(2, 4, 1);

        note1 = new Note(1, 0);
        note2 = new Note(3, 0);
        note3 = new Note(5, 0);
        pos->m_noteArray.push_back(note1);
        pos->m_noteArray.push_back(note2);
        pos->m_noteArray.push_back(note3);

        pos->SetArpeggioUp(true);
        pos->SetPickStrokeDown(true);
        pos->SetTremoloBar(Position::dip, 4, 12);
    }

    ~PositionFixture()
    {
        delete pos;
    }

    Note *note1, *note2, *note3;
    Position* pos;
};

BOOST_AUTO_TEST_SUITE(PositionTest)

    BOOST_FIXTURE_TEST_CASE(GetNoteByString, PositionFixture)
    {
        BOOST_CHECK_EQUAL(pos->GetNoteByString(2), (Note*)NULL);
        BOOST_CHECK_EQUAL(pos->GetNoteByString(1), note1);
        BOOST_CHECK_EQUAL(pos->GetNoteByString(3), note2);
    }

    BOOST_AUTO_TEST_SUITE(GetStringBounds)

        BOOST_FIXTURE_TEST_CASE(MultipleStrings, PositionFixture)
        {
            BOOST_CHECK(pos->GetStringBounds() == (std::pair<uint8_t, uint8_t>(1, 5)));
        }

        BOOST_AUTO_TEST_CASE(SingleString)
        {
            Position pos;
            pos.m_noteArray.push_back(new Note(4, 13));

            BOOST_CHECK(pos.GetStringBounds() == (std::pair<uint8_t, uint8_t>(4, 4)));
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_CASE(Equality)
    {
        PositionFixture pos1;
        PositionFixture pos2;

        BOOST_CHECK(*(pos1.pos) == *(pos2.pos));
    }

    BOOST_FIXTURE_TEST_CASE(CloneObject, PositionFixture)
    {
        Position* pos2 = pos->CloneObject();

        BOOST_CHECK_NE(pos, pos2);
        BOOST_CHECK(*pos == *pos2);

        delete pos2;
    }

    BOOST_FIXTURE_TEST_CASE(Assignment, PositionFixture)
    {
        Position* pos2 = new Position;

        BOOST_CHECK(*pos != *pos2);

        *pos2 = *pos;
        BOOST_CHECK(*pos == *pos2);

        delete pos2;
    }

BOOST_AUTO_TEST_SUITE_END()
