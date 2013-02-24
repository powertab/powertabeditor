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
  
#include "fileformat.h"

FileFormat::FileFormat(const std::string &name,
                       const std::vector<std::string> &fileExtensions) :
    name(name),
    fileExtensions(fileExtensions)
{
}

bool FileFormat::operator<(const FileFormat& format) const
{
    return name < format.name;
}

/// Returns a correctly formatted file filter for a Qt file dialog.
/// e.g. "FileType (*.ext1 *.ext2)".
std::string FileFormat::fileFilter() const
{
    return name + " (" + allExtensions() + ")";
}

std::string FileFormat::allExtensions() const
{
    std::string extensions;

    for (size_t i = 0; i < fileExtensions.size(); ++i)
    {
        if (i != 0)
            extensions += " ";
        extensions += "*." + fileExtensions[i];
    }

    return extensions;
}

bool FileFormat::contains(const std::string &extension) const
{
    return std::find(fileExtensions.begin(), fileExtensions.end(),
                     extension) != fileExtensions.end();
}

FileFormatImporter::FileFormatImporter(const FileFormat& format) :
    format(format)
{
}

FileFormatImporter::~FileFormatImporter()
{
}

FileFormat FileFormatImporter::fileFormat() const
{
    return format;
}

FileFormatException::FileFormatException(const std::string& error) :
    std::runtime_error(error)
{
}


FileFormatExporter::FileFormatExporter(const FileFormat &format) :
    format(format)
{
}

FileFormatExporter::~FileFormatExporter()
{
}

FileFormat FileFormatExporter::fileFormat() const
{
    return format;
}
