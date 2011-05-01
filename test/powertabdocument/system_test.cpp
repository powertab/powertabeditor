#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/system.h>
#include <powertabdocument/staff.h>

BOOST_AUTO_TEST_SUITE(SystemTest)

    BOOST_AUTO_TEST_SUITE(FindStaffIndex)

        BOOST_AUTO_TEST_CASE(noStaves)
        {
            System emptySystem;
            std::shared_ptr<Staff> staff;

            BOOST_CHECK_THROW(emptySystem.FindStaffIndex(staff), std::out_of_range);
        }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
