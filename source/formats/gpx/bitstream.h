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
  
#ifndef GPX_BITSTREAM_H
#define GPX_BITSTREAM_H

#include <iosfwd>
#include <boost/cstdint.hpp>
#include <vector>

namespace Gpx
{

/// Provides the ability to read individual bits from a stream
/// This is required for the compression scheme used in .gpx files
class BitStream
{
public:
    enum BitOrder
    {
        Normal,
        Reversed
    };

    BitStream(std::istream& stream);

    uint32_t readInt();
    uint8_t readBit();
    int32_t readBits(int n, BitOrder = Normal);

    uint32_t location() const;
    bool atEnd() const;

private:
    uint32_t position; /// current position, in bits
    std::vector<char> bytes; /// compressed data being read
    static const uint32_t BYTE_LENGTH = 8;
};

}

#endif // GPX_BITSTREAM_H
