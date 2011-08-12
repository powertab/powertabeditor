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
  
#ifndef SERIALIZATION_TEST_H
#define SERIALIZATION_TEST_H

#include <boost/test/unit_test.hpp>
#include <sstream>
#include <powertabdocument/powertabinputstream.h>
#include <powertabdocument/powertaboutputstream.h>
#include <powertabdocument/powertabfileheader.h>

/// Serialize and then deserialize the given object, and then check for equality
template <typename T>
void testSerialization(const T& originalObject)
{
    std::ostringstream outputStrStream(std::ostringstream::binary);
    PowerTabOutputStream outputStream(outputStrStream);
    originalObject.Serialize(outputStream);

    std::istringstream inputStrStream(outputStrStream.str(), std::istringstream::binary);
    PowerTabInputStream inputStream(inputStrStream);

    T newObject;
    newObject.Deserialize(inputStream, PowerTabFileHeader::FILEVERSION_CURRENT);

    BOOST_CHECK(originalObject == newObject);
}

#endif // SERIALIZATION_TEST_H
