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

#include "bitstream.h"

#include "util.h"
#include <cassert>
#include <istream>

static constexpr uint32_t BYTE_LENGTH = 8;

Gpx::BitStream::BitStream(std::istream &stream) : myPosition(0)
{
    // Copy data from the stream into an internal buffer.
    stream.seekg(0, std::ios::end);
    myBytes.resize(stream.tellg());

    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<char *>(&myBytes[0]), myBytes.size());
}

uint32_t
Gpx::BitStream::readInt()
{
    assert(myPosition % BYTE_LENGTH == 0);

    const uint32_t value =
        Gpx::Util::readUInt(myBytes, myPosition / BYTE_LENGTH);
    myPosition += sizeof(uint32_t) * BYTE_LENGTH;
    return value;
}

bool
Gpx::BitStream::readBit()
{
    if (myPosition / BYTE_LENGTH >= myBytes.size())
        return 0;

    std::byte byte = myBytes[myPosition / BYTE_LENGTH];
    byte >>= ((BYTE_LENGTH - 1) - (myPosition % BYTE_LENGTH));
    byte &= std::byte{ 0x01 };

    ++myPosition;

    return byte != std::byte{ 0 };
}

int32_t
Gpx::BitStream::readBits(int n, BitOrder order)
{
    int32_t value = 0;

    if (order == Reversed)
    {
        for (int i = 0; i < n; ++i)
            value |= (readBit() << i);
    }
    else
    {
        for (int i = n - 1; i >= 0; --i)
            value |= (readBit() << i);
    }

    return value;
}

size_t
Gpx::BitStream::getLocation() const
{
    return myPosition / BYTE_LENGTH;
}

bool
Gpx::BitStream::isAtEnd() const
{
    return getLocation() >= (myBytes.size() - 1);
}
