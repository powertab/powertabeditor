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

#include <powertabdocument/keysignature.h>

BOOST_AUTO_TEST_SUITE(KeySignatureTest)

    BOOST_AUTO_TEST_CASE(GetText)
    {
        KeySignature key(KeySignature::majorKey, KeySignature::fourFlats);

        BOOST_CHECK_EQUAL(key.GetText(), "Ab Major - Bb Eb Ab Db");

        key.SetKey(KeySignature::majorKey, KeySignature::noAccidentals);
        BOOST_CHECK_EQUAL(key.GetText(), "C Major");

        key.SetKey(KeySignature::minorKey, KeySignature::twoSharps);
        BOOST_CHECK_EQUAL(key.GetText(), "B Minor - F# C#");
    }

BOOST_AUTO_TEST_SUITE_END()


