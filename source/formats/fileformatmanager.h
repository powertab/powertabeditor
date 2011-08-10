#ifndef FILEFORMATMANAGER_H
#define FILEFORMATMANAGER_H

// Workaround for Boost warning that only appears on Windows
// (see https://svn.boost.org/trac/boost/ticket/4276)
#ifdef __MINGW32__
#pragma GCC diagnostic ignored "-Wignored-qualifiers" 
#endif
#include <boost/ptr_container/ptr_map.hpp>

#include <vector>
#include "fileformat.h"

class FileFormatImporter;
class FileFormatExporter;

/// An interface for import/export of various file formats
class FileFormatManager
{
public:
    FileFormatManager();
    ~FileFormatManager();

    std::vector<FileFormat> importedFileFormats() const;
    std::shared_ptr<PowerTabDocument> import(const std::string& fileName, const FileFormat& format);

private:
    boost::ptr_map<FileFormat, FileFormatImporter> importers;
    boost::ptr_map<FileFormat, FileFormatExporter> exporters;
};

#endif // FILEFORMATMANAGER_H
