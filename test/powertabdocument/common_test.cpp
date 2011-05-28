#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/common.h>
#include <vector>
#include <memory>

BOOST_AUTO_TEST_CASE(DeepCopyAndEquality)
{
    std::vector<std::shared_ptr<int> > vec1, vec2;
    
    vec1.push_back(std::shared_ptr<int>(new int(5)));
    vec1.push_back(std::shared_ptr<int>(new int(7)));
    
    deepCopy(vec1, vec2);

    BOOST_CHECK_EQUAL(vec1.size(), vec2.size()); 
    BOOST_CHECK_EQUAL(*vec1[0], *vec2[0]);
    BOOST_CHECK(isDeepEqual(vec1, vec2));

    *vec2[0] = 6;
    BOOST_CHECK_NE(*vec1[0], *vec2[0]);
    BOOST_CHECK(!isDeepEqual(vec1, vec2));
}

