#ifndef GPX_FILESYSTEM_H
#define GPX_FILESYSTEM_H

#include <string>
#include <vector>
#include <map>

namespace Gpx
{

/// The uncompressed *.gpx file is essentially a filesystem containing several xml files
/// This class handles the retrieval of information from that filesystem
class FileSystem
{
public:
    FileSystem(std::vector<char> data);

    std::vector<char> getFileContents(const std::string& fileName) const;

private:
    std::map<std::string, std::vector<char> > files;

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
