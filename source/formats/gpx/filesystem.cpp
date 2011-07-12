#include "filesystem.h"

#include "util.h"
#include "bitstream.h"

#include <formats/fileformat.h>

#include <cassert>

Gpx::FileSystem::FileSystem(std::vector<char> data)
{
    data.erase(data.begin(), data.begin() + 4); // skip BCFS header
    size_t offset = 0;

    // read all file from the file system
    while ( (offset = (offset + SECTOR_SIZE)) + 3 < data.size())
    {
        if (Util::readUInt(data, offset) == 2)
        {
            const size_t fileNameIndex = offset + 4;
            const size_t fileSizeIndex= offset + 0x8C;
            const size_t blockIndex= offset + 0x94;

            int block = 0;
            int blockCount = 0;
            std::vector<char> fileData;

            // read the file data
            while ((block = Util::readUInt(data, blockIndex + 4 * blockCount)) != 0)
            {
                offset = block * SECTOR_SIZE;
                std::copy(data.begin() + offset, data.begin() + offset + SECTOR_SIZE,
                          std::back_inserter(fileData));
                blockCount++;
            }

            // read the file name and save the file
            const uint32_t fileSize = Util::readUInt(data, fileSizeIndex);
            if (fileData.size() >= fileSize)
            {
                std::string fileName;
                std::copy(data.begin() + fileNameIndex, data.begin() + fileNameIndex + 127,
                          std::back_inserter(fileName));
                fileName.erase(fileName.find_last_not_of('\0') + 1); // trim extra NULL characters

                std::vector<char> file(fileData.begin(), fileData.begin() + fileSize);

                files[fileName] = file;
            }
        }
    }
}

/// Decompress the input file and return the filesystem
Gpx::FileSystem Gpx::load(std::istream& stream)
{
    Gpx::BitStream input(stream);

    const uint32_t BCFS_HEADER = 0x53464342;
    const uint32_t BCFZ_HEADER = 0x5a464342;

    std::vector<char> output;

    uint32_t header = input.readInt();
    //std::cerr << "Header: " << header << std::endl;

    assert(header == BCFZ_HEADER); // should be a compressed file
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

    const uint32_t newHeader = Gpx::Util::readUInt(output, 0);
    if (newHeader != BCFS_HEADER)
    {
        throw FileFormatException("Invalid GPX Format");
    }

    return Gpx::FileSystem(output);
}

std::vector<char> Gpx::FileSystem::getFileContents(const std::string& fileName) const
{
    auto file = files.find(fileName);

    if (file == files.end())
    {
        throw FileFormatException("Invalid filename");
    }

    return file->second;
}
