#include "fileformat.h"

FileFormat::FileFormat(const std::string &name,
                       const std::string &fileExtensions) :
    name(name),
    fileExtensions(fileExtensions)
{
}

bool FileFormat::operator<(const FileFormat& format) const
{
    return name < format.name;
}

FileFormatImporter::FileFormatImporter(const FileFormat& format) :
    format(format)
{
}

FileFormatImporter::~FileFormatImporter()
{
}

FileFormat FileFormatImporter::getFileFormat() const
{
    return format;
}

FileFormatException::FileFormatException(const std::string& error) :
    std::runtime_error(error)
{
}
