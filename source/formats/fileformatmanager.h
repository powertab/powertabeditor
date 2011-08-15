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
    boost::shared_ptr<PowerTabDocument> import(const std::string& fileName, const FileFormat& format);

private:
    boost::ptr_map<FileFormat, FileFormatImporter> importers;
    boost::ptr_map<FileFormat, FileFormatExporter> exporters;
};

#endif // FILEFORMATMANAGER_H
