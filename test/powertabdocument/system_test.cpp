#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/system.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/barline.h>

BOOST_AUTO_TEST_SUITE(SystemTest)

    BOOST_AUTO_TEST_SUITE(FindStaffIndex)

        BOOST_AUTO_TEST_CASE(noStaves)
        {
            System emptySystem;
            std::shared_ptr<Staff> staff;

            BOOST_CHECK_THROW(emptySystem.FindStaffIndex(staff), std::out_of_range);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_CASE(CopyAndEquality)
    {
        System system1;
        system1.InsertBarline(System::BarlinePtr(new Barline(2, Barline::doubleBar, 0)));
        System system2 = system1;

        BOOST_CHECK(system1 == system2);

        // check deep copy
        system1.GetBarlineAtPosition(2)->SetBarlineData(Barline::repeatStart, 0);
        BOOST_CHECK(system1 != system2);
    }

BOOST_AUTO_TEST_SUITE_END()
