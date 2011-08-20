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
  
#include "inputstream.h"

#include <cassert>

Gp::InputStream::InputStream(std::istream& stream) :
    stream_(stream)
{
    stream_.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);
}

/// Reads the file version
std::string Gp::InputStream::readVersionString()
{
    stream_.seekg(std::ios_base::beg);

    // the version consists of a 30 character string, although not all 30
    // characters may be used
    std::string version = readCharacterString<uint8_t>();

    // skip past any unread characters to land at position 0x1f
    stream_.seekg(31, std::ios_base::beg);

    return version;
}

/// Reads a string in the most common format for Guitar Pro - an integer representing the
/// size of the stored information + 1, followed by the length-prefixed string of characters
/// representing the data
std::string Gp::InputStream::readString()
{
    const uint32_t size = read<uint32_t>();

    std::string str = readCharacterString<uint8_t>();
    assert(size - 1 == str.length());

    return str;
}

/// Reads a string prefixed with 4 bytes indicating the length
std::string Gp::InputStream::readIntString()
{
    return readCharacterString<uint32_t>();
}

/// Reads a fixed length string (any unused characters trailing the string are skipped)
std::string Gp::InputStream::readFixedLengthString(uint32_t maxLength)
{
    const uint8_t actualLength = read<uint8_t>();

    std::string str;

    if (maxLength != 0)
    {
        str.resize(maxLength);
    }
    else
    {
        str.resize(actualLength);
    }

    stream_.read(&str[0], str.size());
    str.resize(actualLength);

    return str;
}

void Gp::InputStream::skip(int numBytes)
{
    stream_.seekg(numBytes, std::ios_base::cur);
}
