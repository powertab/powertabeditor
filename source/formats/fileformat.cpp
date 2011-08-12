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
