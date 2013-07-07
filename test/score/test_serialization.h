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

#ifndef TEST_SCORE_SERIALIZATION_H
#define TEST_SCORE_SERIALIZATION_H

#include <catch.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>

namespace Serialization {

    /// Basic test for the serialization code - we should be able to serialize
    /// and deserialize and object, and get an equivalent object back.
    template <typename T>
    void test(const T &original)
    {
        std::ostringstream output;
        boost::archive::text_oarchive oa(output);
        oa << original;

        T copy;
        std::istringstream input(output.str());
        boost::archive::text_iarchive ia(input);
        ia >> copy;

        REQUIRE(original == copy);
    }
}

#endif
