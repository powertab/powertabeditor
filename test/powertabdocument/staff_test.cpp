#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

// Provides a staff with some positions already inserted
struct StaffFixture
{
    StaffFixture()
    {
        positions.push_back(new Position(0, 4, 0));
        positions.push_back(new Position(2, 4, 0));
        positions.push_back(new Position(4, 4, 0));
        positions.push_back(new Position(5, 4, 0));
        positions.push_back(new Position(6, 4, 0));

        // put some sample notes at each position
        positions.at(0)->m_noteArray.push_back(new Note(1, 4));
        positions.at(0)->m_noteArray.push_back(new Note(2, 3));
        positions.at(1)->m_noteArray.push_back(new Note(1, 5));
        positions.at(2)->m_noteArray.push_back(new Note(1, 5));
        positions.at(3)->m_noteArray.push_back(new Note(1, 6));
        positions.at(4)->m_noteArray.push_back(new Note(1, 5));

        staff.highMelodyPositionArray = positions;
    }

    ~StaffFixture() {}

    Staff staff;
    std::vector<Position*> positions;
};

// Provides a blank staff
struct EmptyStaffFixture
{
    EmptyStaffFixture() {}
    ~EmptyStaffFixture() {}

    Staff staff;
};

BOOST_AUTO_TEST_SUITE(TestStaff)

    // tests for clef-related functions
    BOOST_AUTO_TEST_SUITE(Clef)

        BOOST_AUTO_TEST_CASE(IsValidClef)
        {
            BOOST_CHECK(Staff::IsValidClef(Staff::BASS_CLEF));
            BOOST_CHECK(Staff::IsValidClef(Staff::TREBLE_CLEF));
            BOOST_CHECK(!Staff::IsValidClef(Staff::BASS_CLEF + 1));
        }

        BOOST_FIXTURE_TEST_CASE(ModifyClef, EmptyStaffFixture)
        {
            staff.SetClef(Staff::BASS_CLEF);
            BOOST_CHECK_EQUAL(staff.GetClef(), Staff::BASS_CLEF);

            staff.SetClef(Staff::TREBLE_CLEF);
            BOOST_CHECK_EQUAL(staff.GetClef(), Staff::TREBLE_CLEF);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE(GetLastPosition)

        BOOST_AUTO_TEST_CASE(empty_staff)
        {
            Staff emptyStaff;
            BOOST_CHECK(emptyStaff.GetLastPosition() == NULL);
        }

        BOOST_AUTO_TEST_CASE(single_position)
        {
            Staff singleItemStaff;
            Position* pos1 = new Position();
            singleItemStaff.highMelodyPositionArray.push_back(pos1);

            BOOST_CHECK_EQUAL(singleItemStaff.GetLastPosition(), pos1);
        }

        BOOST_FIXTURE_TEST_CASE(multiple_positions, StaffFixture)
        {
            BOOST_CHECK_EQUAL(staff.GetLastPosition(), positions.at(positions.size() - 1));
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_FIXTURE_TEST_SUITE(CanHammerOn, StaffFixture)

        BOOST_AUTO_TEST_CASE(LessThanNextFretOkay)
        {
            Position* pos = positions.at(0);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)));
        }

        BOOST_AUTO_TEST_CASE(EqualFretsNotOkay)
        {
            Position* pos = positions.at(1);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)) == false);
        }

        BOOST_AUTO_TEST_CASE(GreaterThanNextFretNotOkay)
        {
            Position* pos = positions.at(3);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)) == false);
        }

        BOOST_AUTO_TEST_CASE(LastNoteCantBeHammeron)
        {
            Position* pos = positions.at(positions.size() - 1);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)) == false);
        }

    BOOST_AUTO_TEST_SUITE_END()

    /// Tests Staff::CanSlideBetweenNotes
    BOOST_FIXTURE_TEST_SUITE(CanSlideBetweenNotes, StaffFixture)

        BOOST_AUTO_TEST_CASE(DifferentFretOkay)
        {
            Position* pos = positions.at(0);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)));

            pos = positions.at(3);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)));
        }

        BOOST_AUTO_TEST_CASE(EqualFretsNotOkay)
        {
            Position* pos = positions.at(1);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == false);
        }

        BOOST_AUTO_TEST_CASE(LastNoteCantSlideToNext)
        {
            Position* pos = positions.at(positions.size() - 1);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == false);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_FIXTURE_TEST_SUITE(GetSlideSteps, StaffFixture)

        BOOST_AUTO_TEST_CASE(Calculation)
        {
            Position* pos = positions.at(0);
            BOOST_CHECK_EQUAL(staff.GetSlideSteps(pos, pos->GetNote(0)), 1);

            pos = positions.at(1);
            BOOST_CHECK_EQUAL(staff.GetSlideSteps(pos, pos->GetNote(0)), 0);

            pos = positions.at(3);
            BOOST_CHECK_EQUAL(staff.GetSlideSteps(pos, pos->GetNote(0)), -1);
        }

        BOOST_AUTO_TEST_CASE(ThrowsWithNoAdjacentNote)
        {
            Position* pos = positions.at(0);
            // use note on the second string, since the next position only has a note on the first string
            BOOST_CHECK_THROW(staff.GetSlideSteps(pos, pos->GetNote(1)), std::logic_error);
        }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
