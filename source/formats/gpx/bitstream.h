#ifndef GPX_BITSTREAM_H
#define GPX_BITSTREAM_H

#include <iosfwd>
#include <cstdint>
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
