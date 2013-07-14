/*
  * Copyright (C) 2013 Cameron White
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

#include <boost/lexical_cast.hpp>
#include <score/alternateending.h>
#include "test_serialization.h"

TEST_CASE("Score/AlternateEnding/Numbers", "")
{
    AlternateEnding ending;
    ending.addNumber(1);
    ending.addNumber(4);
    ending.addNumber(3);
    ending.removeNumber(1);
    ending.addNumber(7);

    std::vector<int> numbers;
    numbers.push_back(3);
    numbers.push_back(4);
    numbers.push_back(7);

    // Should be sorted from low to high.
    REQUIRE(ending.getNumbers() == numbers);

    // Cannot add duplicates.
    REQUIRE_THROWS(ending.addNumber(7));
    // Cannot add more than 8.
    REQUIRE_THROWS(ending.addNumber(9));
}

TEST_CASE("Score/AlternateEnding/SpecialEndings", "")
{
    AlternateEnding ending;

    REQUIRE(!ending.hasDaCapo());
    ending.setDaCapo(true);
    REQUIRE(ending.hasDaCapo());

    REQUIRE(!ending.hasDalSegno());
    ending.setDalSegno(true);
    REQUIRE(ending.hasDalSegno());

    REQUIRE(!ending.hasDalSegnoSegno());
    ending.setDalSegnoSegno(true);
    REQUIRE(ending.hasDalSegnoSegno());
}

TEST_CASE("Score/AlternateEnding/ToString", "")
{
    AlternateEnding ending;

    REQUIRE(boost::lexical_cast<std::string>(ending) == "");

    ending.addNumber(1);
    REQUIRE(boost::lexical_cast<std::string>(ending) == "1.");

    ending.addNumber(2);
    REQUIRE(boost::lexical_cast<std::string>(ending) == "1., 2.");

    ending.addNumber(3);
    REQUIRE(boost::lexical_cast<std::string>(ending) == "1., 2., 3.");

    ending = AlternateEnding();
    ending.addNumber(1);
    ending.addNumber(5);
    ending.setDalSegno();
    REQUIRE(boost::lexical_cast<std::string>(ending) == "1., 5., D.S.");
}

TEST_CASE("Score/AlternateEnding/Serialization", "")
{
    AlternateEnding ending;
    ending.setPosition(42);
    ending.addNumber(2);
    ending.addNumber(5);
    ending.setDalSegno(true);

    Serialization::test(ending);
}
