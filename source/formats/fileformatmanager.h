#ifndef FILEFORMATMANAGER_H
#define FILEFORMATMANAGER_H

#include <boost/ptr_container/ptr_map.hpp>
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
