/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
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

