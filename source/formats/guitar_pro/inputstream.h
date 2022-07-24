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

#ifndef FORMATS_GP_STREAM_H
#define FORMATS_GP_STREAM_H

#include <boost/endian/conversion.hpp>
#include <cstdint>
#include <istream>
#include <vector>
#include <util/toutf8.h>

#include "document.h"

namespace Gp
{
class InputStream
{
public:
    InputStream(std::istream &stream);

    /// Returns the file version.
    Version version() const;

    /// Reads simple data (e.g. uint32_t, int16_t) from the input stream.
    template <class T>
    [[nodiscard]] T read();

    /// Reads a bool (stored in the file as uint8_t).
    [[nodiscard]] bool readBool();

    /// Reads a string in the most common format for Guitar Pro - an integer
    /// representing the size of the stored information + 1, followed by the
    /// length-prefixed string of characters representing the data
    std::string readString();

    /// Reads a string prefixed with 4 bytes that indicate the length.
    std::string readIntString();

    /// Reads a fixed length string (any unused characters trailing the string
    /// are skipped).
    std::string readFixedLengthString(uint32_t maxLength);

    std::string readVersionString();

    void skip(int numBytes);

private:
    /// Reads a character string.
    /// The string consists of some number of bytes (encoding the length of the
    /// string, n) followed by n characters.  This is templated on the length
    /// prefix type, to allow for strings prefixed with a 2-byte length value,
    /// 4-byte length value, etc
    template <class LengthPrefixType>
    std::string readCharacterString();

    std::istream &myStream;
    Version myVersion;
};

template <class T>
inline T InputStream::read()
{
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    T data;
    myStream.read((char *)&data, sizeof(data));
    // The values are stored in little-endian format.
    return boost::endian::little_to_native(data);
}

inline bool
InputStream::readBool()
{
    return read<uint8_t>() != 0;
}

template <typename LengthPrefixType>
inline std::string InputStream::readCharacterString()
{
    static_assert(std::is_integral<LengthPrefixType>::value,
                  "LengthPrefixType must be an integral type");

    const LengthPrefixType length = read<LengthPrefixType>();

    std::string str;
    str.resize(length);

    if (length != 0)
        myStream.read(&str[0], length);

    Util::convertISO88591ToUTF8(str);

    return str;
}

inline Version
InputStream::version() const
{
    return myVersion;
}
}

#endif
