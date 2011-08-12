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
#include <boost/foreach.hpp>

#include <powertabdocument/alternateending.h>

BOOST_AUTO_TEST_SUITE(AlternateEndingTest)

    BOOST_AUTO_TEST_CASE(GetText)
    {
        AlternateEnding ending;

        // empty
        BOOST_CHECK_EQUAL(ending.GetText(), "");

        ending.SetNumber(1);
        BOOST_CHECK_EQUAL(ending.GetText(), "1.");

        ending.SetNumber(2);
        BOOST_CHECK_EQUAL(ending.GetText(), "1., 2.");

        ending.SetNumber(3);
        BOOST_CHECK_EQUAL(ending.GetText(), "1.-3.");

        ending.SetNumbers(1 | 16 | 32 | 64 | 512);
        BOOST_CHECK_EQUAL(ending.GetText(), "1., 5.-7., D.S.");
    }

    BOOST_AUTO_TEST_CASE(GetListOfNumbers)
    {
        AlternateEnding ending;

        std::vector<uint8_t> originalNumbers;
        originalNumbers.push_back(1);
        originalNumbers.push_back(3);
        originalNumbers.push_back(4);
        originalNumbers.push_back(7);

        BOOST_FOREACH(uint8_t number, originalNumbers)
        {
            ending.SetNumber(number);
        }

        std::vector<uint8_t> numbers = ending.GetListOfNumbers();

        BOOST_CHECK_EQUAL_COLLECTIONS(numbers.begin(), numbers.end(),
                                      originalNumbers.begin(), originalNumbers.end());
    }

BOOST_AUTO_TEST_SUITE_END()
