#include "bitstream.h"

#include "util.h"
#include <istream>
#include <cassert>

Gpx::BitStream::BitStream(std::istream& stream) :
    position(0)
{
    // copy data from the stream into an internal buffer
    stream.seekg(0, std::ios::end);
    bytes.resize(stream.tellg());

    stream.seekg(0, std::ios::beg);
    stream.read(&bytes[0], bytes.size());
}

/// Reads a 32-bit integer - assumes that the stream position is exactly on the start of a byte
uint32_t Gpx::BitStream::readInt()
{
    assert(position % BYTE_LENGTH == 0);

    const uint32_t value = Gpx::Util::readUInt(bytes, position / BYTE_LENGTH);
    position += sizeof(uint32_t) * BYTE_LENGTH;
    return value;
}

/// Reads a single bit
uint8_t Gpx::BitStream::readBit()
{
    if (position / BYTE_LENGTH >= bytes.size())
        return 0;

    char byte = bytes.at(position / BYTE_LENGTH);
    byte >>= ((BYTE_LENGTH - 1) - (position % BYTE_LENGTH));
    byte &= 0x01;

    position++;

    return byte;
}

/// Reads an integer of the specified number of bits
int32_t Gpx::BitStream::readBits(int n, BitOrder order)
{
    int32_t value = 0;

    if (order == Reversed)
    {
        for (int i = 0; i < n; i++)
        {
            value |= (readBit() << i);
        }
    }
    else
    {
        for (int i = n - 1; i >= 0; i--)
        {
            value |= (readBit() << i);
        }
    }

    return value;
}

/// Returns the location of the stream, in bytes
uint32_t Gpx::BitStream::location() const
{
    return position / BYTE_LENGTH;
}

/// Returns true if the stream has reached the end
bool Gpx::BitStream::atEnd() const
{
    return location() >= (bytes.size() - 1);
}
