#include "bitstream.h"
#include "filesystem.h"
#include "util.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <iterator>

enum ChunkHeader
{
    Uncompressed,
    Compressed
};

Gpx::FileSystem load(Gpx::BitStream& input)
{
    const uint32_t BCFS = 0x53464342;
    const uint32_t BCFZ = 0x5a464342;

    std::vector<char> output;

    uint32_t header = input.readInt();
    //std::cerr << "Header: " << header << std::endl;

    assert(header == BCFZ); // should be a compressed file
    uint32_t length = input.readInt();
    output.reserve(length);

    // We now have a succession of compressed/uncompressed chunks
    while (!input.atEnd() && input.location() < length)
    {
        const uint8_t chunkHeader = input.readBit();

        // For an uncompressed chunk, we have 2 bits containing the length in bytes of the content,
        // followed by the content itself
        if (chunkHeader == Uncompressed)
        {
            const int32_t rawLength = input.readBits(2, Gpx::BitStream::Reversed);

            //std::cerr << "Uncompressed: " << rawLength << std::endl;

            for (int32_t i = 0; i < rawLength; i++)
            {
                output.push_back(input.readBits(8));
            }
        }
        // For a compressed chunk, we have a 4-bit integer giving a length P, then two integers of
        // P bits representing the offset and length of the data (in bytes) to read
        // We then copy the already-read uncompressed data from the given offset
        else if (chunkHeader == Compressed)
        {
            const uint32_t p = input.readBits(4);
            const int32_t offset = input.readBits(p, Gpx::BitStream::Reversed);
            const int32_t length = input.readBits(p, Gpx::BitStream::Reversed);

            //std::cerr << "Compressed: offset = " << offset << ", length = " << length << std::endl;

            const uint32_t startPos = output.size() - offset;

            std::copy(output.begin() + startPos,
                      output.begin() + startPos + length,
                      std::back_inserter(output));
        }
    }

    std::cerr << output.size() << std::endl;
    //std::copy(output.begin(), output.end(), std::ostream_iterator<char>(std::cout, ""));

    const uint32_t newHeader = Gpx::Util::readUInt(output, 0);
    if (newHeader != BCFS)
    {
        std::cerr << "Error" << std::endl;
    }

    return Gpx::FileSystem(output);
}

int main(int, char *[])
{
    std::ifstream file("btbamsong.gpx", std::ios::binary | std::ios::in);
    Gpx::BitStream stream(file);
    load(stream);

    return 0;
}
