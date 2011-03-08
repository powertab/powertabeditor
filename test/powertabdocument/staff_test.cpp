#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <powertabdocument/staff.h>

// Provides a staff with some positions already inserted
struct StaffFixture
{
    StaffFixture()
    {
        firstPos = new Position(0, 4, 0);
        staff.highMelodyPositionArray.push_back(firstPos);

        lastPos = new Position(5, 4, 0);
        staff.highMelodyPositionArray.push_back(lastPos);
    }

    ~StaffFixture() {}

    Staff staff;
    Position* lastPos;
    Position* firstPos;
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
            BOOST_CHECK_EQUAL(staff.GetLastPosition(), lastPos);
        }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
