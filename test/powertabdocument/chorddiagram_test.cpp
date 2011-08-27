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

#include "serialization_test.h"
#include <powertabdocument/chorddiagram.h>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

BOOST_AUTO_TEST_SUITE(ChordDiagramTest)

    BOOST_AUTO_TEST_CASE(Serialization)
    {
        ChordDiagram diagram(7, list_of(1)(2)(3)(4));
        testSerialization(diagram);
    }

    BOOST_AUTO_TEST_CASE(Spelling)
    {
        ChordDiagram diagram(3, list_of(3)(4)(5)(5)(3)(ChordDiagram::stringMuted));
        BOOST_CHECK_EQUAL(diagram.GetSpelling(), "x 3 5 5 4 3");
    }

    BOOST_AUTO_TEST_CASE(IsSameVoicing)
    {
        ChordDiagram diagram(ChordName(), 3, list_of(3)(4)(5)(5)(3));

        std::vector<uint8_t> voicing = list_of(3)(4)(5)(5)(3);
        BOOST_CHECK(diagram.IsSameVoicing(voicing));

        voicing[0] = 4;
        BOOST_CHECK(!diagram.IsSameVoicing(voicing)); // different fret

        voicing.push_back(3);
        BOOST_CHECK(!diagram.IsSameVoicing(voicing)); // different # of strings
    }

BOOST_AUTO_TEST_SUITE_END()
