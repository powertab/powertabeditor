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
  
#include "filesystem.h"

#include "bitstream.h"
#include <boost/algorithm/clamp.hpp>
#include <cassert>
#include <formats/fileformat.h>
#include "util.h"

enum ChunkHeader
{
    Uncompressed = 0,
    Compressed = 1
};

static const uint32_t SECTOR_SIZE = 0x1000;

Gpx::FileSystem::FileSystem(std::istream &stream)
{
    // Decompress the input file and return the filesystem.
    Gpx::BitStream input(stream);

    const uint32_t BCFS_HEADER = 0x53464342;
    const uint32_t BCFZ_HEADER = 0x5a464342;

    const uint32_t header = input.readInt();

    // This should be a compressed file right now.
    if (header != BCFZ_HEADER)
        throw FileFormatException("Invalid header");

    const uint32_t length = input.readInt();
    std::vector<uint8_t> output;
    output.reserve(length);

    // We now have a succession of compressed and uncompressed chunks.
    while (!input.isAtEnd() && input.getLocation() < length)
    {
        const ChunkHeader chunkHeader = static_cast<ChunkHeader>(input.readBit());

        // For an uncompressed chunk, we have 2 bits containing the length in
        // bytes of the content, followed by the content itself.
        if (chunkHeader == Uncompressed)
        {
            const int32_t rawLength = input.readBits(2, Gpx::BitStream::Reversed);

            for (int32_t i = 0; i < rawLength; ++i)
                output.push_back(input.readBits(8));
        }
        // For a compressed chunk, we have a 4-bit integer giving a length P,
        // then two integers of P bits representing the offset and length of the
        // data (in bytes) to read.
        // We then copy the already-read uncompressed data from the given offset.
        else if (chunkHeader == Compressed)
        {
            const int32_t p = input.readBits(4);
            const int32_t offset = input.readBits(p, Gpx::BitStream::Reversed);
            const uint32_t startPos = output.size() - offset;

            const int32_t length = boost::algorithm::clamp<int32_t>(
                        input.readBits(p, Gpx::BitStream::Reversed), 0,
                        output.size() - startPos);

            std::copy(output.begin() + startPos,
                      output.begin() + startPos + length,
                      std::back_inserter(output));
        }
    }

    // The data we just read should now have a header indicating that it's
    // uncompressed!
    const uint32_t newHeader = Gpx::Util::readUInt(output, 0);
    if (newHeader != BCFS_HEADER)
        throw FileFormatException("Invalid GPX Format");

    readUncompressedData(output);
}

const std::string &Gpx::FileSystem::getFileContents(
        const std::string &filename) const
{
    std::map<std::string, std::string>::const_iterator file = myFiles.find(filename);

    if (file == myFiles.end())
        throw FileFormatException("Invalid filename");
    else
        return file->second;
}

void Gpx::FileSystem::readUncompressedData(std::vector<uint8_t> &data)
{
    // Remove the BCFS header.
    data.erase(data.begin(), data.begin() + 4);
    size_t offset = 0;

    // Read all files from the file system.
    while ( (offset = (offset + SECTOR_SIZE)) + 3 < data.size())
    {
        if (Util::readUInt(data, offset) == 2)
        {
            const size_t fileNameIndex = offset + 4;
            const size_t fileSizeIndex= offset + 0x8C;
            const size_t blockIndex= offset + 0x94;

            int block = 0;
            int blockCount = 0;
            std::vector<uint8_t> fileData;

            // Read the file data.
            while ((block = Util::readUInt(data,
                                           blockIndex + 4 * blockCount)) != 0)
            {
                offset = block * SECTOR_SIZE;
                std::copy(data.begin() + offset,
                          data.begin() +
                          boost::algorithm::clamp<size_t>(offset + SECTOR_SIZE,
                                                          0, data.size()),
                          std::back_inserter(fileData));
                ++blockCount;
            }

            // Read the file name and save the file.
            const uint32_t fileSize = Util::readUInt(data, fileSizeIndex);
            if (fileData.size() >= fileSize)
            {
                std::string fileName;
                std::copy(data.begin() + fileNameIndex,
                          data.begin() + fileNameIndex + 127,
                          std::back_inserter(fileName));
                // Trim extra NULL characters.
                fileName.erase(fileName.find_last_not_of('\0') + 1);

                std::string file(fileData.begin(), fileData.begin() + fileSize);
                myFiles[fileName] = file;
            }
        }
    }
}
