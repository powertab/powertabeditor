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

#include <powertabdocument/keysignature.h>

TEST_CASE("PowerTabDocument/KeySignature/GetText", "")
{
    KeySignature key(KeySignature::majorKey, KeySignature::fourFlats);

    REQUIRE(key.GetText() == "Ab Major - Bb Eb Ab Db");

    key.SetKey(KeySignature::majorKey, KeySignature::noAccidentals);
    REQUIRE(key.GetText() == "C Major");

    key.SetKey(KeySignature::minorKey, KeySignature::twoSharps);
    REQUIRE(key.GetText() == "B Minor - F# C#");
}
