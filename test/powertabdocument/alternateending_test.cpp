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

#include <catch.hpp>

#include <boost/foreach.hpp>
#include <powertabdocument/alternateending.h>

TEST_CASE("PowerTabDocument/AlternateEnding/GetText", "")
{
    AlternateEnding ending;

    // empty
    REQUIRE(ending.GetText() == "");

    ending.SetNumber(1);
    REQUIRE(ending.GetText() == "1.");

    ending.SetNumber(2);
    REQUIRE(ending.GetText() == "1., 2.");

    ending.SetNumber(3);
    REQUIRE(ending.GetText() == "1.-3.");

    ending.SetNumbers(1 | 16 | 32 | 64 | 512);
    REQUIRE(ending.GetText() == "1., 5.-7., D.S.");
}

TEST_CASE("PowerTabDocument/AlternateEnding/GetListOfNumbers", "")
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

    REQUIRE(std::equal(numbers.begin(), numbers.end(), originalNumbers.begin()));
}
