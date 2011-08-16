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
  
#ifndef GP_STREAM_H
#define GP_STREAM_H

#include <istream>
#include <vector>
#include <boost/cstdint.hpp>
#include <cassert>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/static_assert.hpp>
#include <bitset>

#include "gp_fileformat.h"

namespace Gp
{

typedef std::bitset<8> Flags;

class InputStream
{
public:
    InputStream(std::istream& stream);

    template <class T>
    T read();

    std::string readString();
    std::string readIntString();
    std::string readFixedLengthString(uint32_t maxLength);

    std::string readVersionString();

    void skip(int numBytes);

    Gp::Version version;

private:
    template <class LengthPrefixType>
    std::string readCharacterString();

    std::istream& stream_;
};

/// Reads simple data (e.g. uint32_t, int16_t) from the input stream
template <class T>
inline T InputStream::read()
{
    BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value); // T must be an arithmetic type
    T data;
    stream_.read((char*)&data, sizeof(data));
    return data;
}

/// Reads a character string.
/// The string consists of some number of bytes (encoding the length of the string, n)
/// followed by n characters
/// This is templated on the length prefix type, to allow for strings prefixed with a 2-byte length value,
/// 4-byte length value, etc
template <typename LengthPrefixType>
inline std::string InputStream::readCharacterString()
{
    BOOST_STATIC_ASSERT(boost::is_integral<LengthPrefixType>::value); // LengthPrefix must be an integral type

    const LengthPrefixType length = read<LengthPrefixType>();

    std::string str;
    str.resize(length);
    stream_.read(&str[0], length);
    return str;
}

}

#endif // GP_STREAM_H
