/*
  * Copyright (C) 2014 Cameron White
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
#include <map>

#include <formats/fileformat.h>

const std::map<std::string, Gp::Version> theVersionStrings = {
    { "FICHIER GUITAR PRO v3.00", Gp::Version3 },
    { "FICHIER GUITAR PRO v4.00", Gp::Version4 },
    { "FICHIER GUITAR PRO v4.06", Gp::Version4 },
    { "FICHIER GUITAR PRO L4.06", Gp::Version4 },
    { "FICHIER GUITAR PRO v5.00", Gp::Version5_0 },
    { "FICHIER GUITAR PRO v5.10", Gp::Version5_1 }
};

Gp::InputStream::InputStream(std::istream &stream) : myStream(stream)
{
    myStream.exceptions(std::istream::failbit | std::istream::badbit |
                       std::istream::eofbit);

    const std::string versionString = readVersionString();

    auto it = theVersionStrings.find(versionString);
    if (it != theVersionStrings.end())
        myVersion = it->second;
    else
        throw FileFormatException("Unsupported file version: " + versionString);
}

std::string Gp::InputStream::readVersionString()
{
    myStream.seekg(std::ios_base::beg);

    // THe version consists of a 30 character string, although not all 30
    // characters may be used.
    std::string version = readCharacterString<uint8_t>();

    // Skip past any unread characters to land at position 0x1f.
    myStream.seekg(31, std::ios_base::beg);

    return version;
}

std::string Gp::InputStream::readString()
{
    [[maybe_unused]] const uint32_t size = read<uint32_t>();

    std::string str = readCharacterString<uint8_t>();
    return str;
}

std::string Gp::InputStream::readIntString()
{
    return readCharacterString<uint32_t>();
}

std::string Gp::InputStream::readFixedLengthString(uint32_t maxLength)
{
    const uint8_t actualLength = read<uint8_t>();

    std::string str;

    if (maxLength != 0)
        str.resize(maxLength);
    else
        str.resize(actualLength);

    if (str.size() != 0)
        myStream.read(&str[0], str.size());

    str.resize(actualLength);
    return str;
}

void Gp::InputStream::skip(int numBytes)
{
    myStream.seekg(numBytes, std::ios_base::cur);
}
