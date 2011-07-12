#ifndef GPX_FILESYSTEM_H
#define GPX_FILESYSTEM_H

#include <string>
#include <vector>

namespace Gpx
{
struct File
{
    File(const std::string& fileName, const std::vector<char>& contents);

    std::string fileName;
    std::vector<char> contents;
};

/// The uncompressed *.gpx file is essentially a filesystem containing several xml files
/// This class handles the retrieval of information from that filesystem
class FileSystem
{
public:
    FileSystem(const std::vector<char>& data);

private:
    std::vector<char> data;
    std::vector<File> files;

    static const uint32_t SECTOR_SIZE = 0x1000;
};

enum ChunkHeader
{
    Uncompressed,
    Compressed
};

FileSystem load(std::istream& stream);

}

#endif // GPX_FILESYSTEM_H
