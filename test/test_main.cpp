#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

bool init_unit_test()
{
    boost::unit_test::framework::master_test_suite().p_name.value = "Power Tab Editor Tests";
    return true;
}

int BOOST_TEST_CALL_DECL
main(int argc, char* argv[])
{
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
