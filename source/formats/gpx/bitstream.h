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

#ifndef FORMATS_GPX_BITSTREAM_H
#define FORMATS_GPX_BITSTREAM_H

#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <vector>

namespace Gpx
{

/// Provides the ability to read individual bits from a stream.
/// This is required for the compression scheme used in .gpx files.
class BitStream
{
public:
    enum BitOrder
    {
        Normal,
        Reversed
    };

    BitStream(std::istream &stream);

    /// Reads a 32-bit unsigned integer from the stream. This assumes that the
    /// stream position is exactly on the start of a byte.
    uint32_t readInt();

    /// Reads the next bit from the stream.
    bool readBit();

    /// Reads the next n bits from the stream into an integer.
    int32_t readBits(int n, BitOrder = Normal);

    /// Returns the position in the stream (measured in bytes).
    size_t getLocation() const;

    /// Returns true if we've reached the end of the stream.
    bool isAtEnd() const;

private:
    /// The current position in the input (measured in bits).
    size_t myPosition;
    /// The compressed data being read.
    std::vector<std::byte> myBytes;
};

} // namespace Gpx

#endif
