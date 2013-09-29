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
#include <score/keysignature.h>
#include "test_serialization.h"

TEST_CASE("Score/KeySignature/ToString", "")
{
    KeySignature key(KeySignature::Major, 4, false);
    REQUIRE(boost::lexical_cast<std::string>(key) == "Ab Major - Bb Eb Ab Db");

    key.setNumAccidentals(0);
    REQUIRE(boost::lexical_cast<std::string>(key) == "C Major");

    key.setKeyType(KeySignature::Minor);
    key.setNumAccidentals(2);
    key.setSharps(true);
    REQUIRE(boost::lexical_cast<std::string>(key) == "B Minor - F# C#");
}

TEST_CASE("Score/KeySignature/Cancellation", "")
{
    KeySignature key(KeySignature::Major, 3, true);
    key.setCancellation(true);

    REQUIRE(key.getNumAccidentals(false) == 0);
    REQUIRE(key.getNumAccidentals(true) == 3);
}

TEST_CASE("Score/KeySignature/Serialization", "")
{
    KeySignature key(KeySignature::Minor, 4, false);
    key.setVisible(false);
    key.setCancellation(true);

    Serialization::test("key_signature", key);
}

