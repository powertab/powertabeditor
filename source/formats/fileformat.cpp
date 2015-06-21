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

#include <algorithm>

FileFormat::FileFormat(const std::string &name,
                       const std::vector<std::string> &fileExtensions)
    : myName(name),
      myFileExtensions(fileExtensions)
{
}

bool FileFormat::operator==(const FileFormat &format) const
{
    return myName == format.myName &&
           myFileExtensions == format.myFileExtensions;
}

std::string FileFormat::fileFilter() const
{
    return myName + " (" + allExtensions() + ")";
}

std::string FileFormat::allExtensions() const
{
    std::string extensions;

    for (size_t i = 0; i < myFileExtensions.size(); ++i)
    {
        if (i != 0)
            extensions += " ";
        extensions += "*." + myFileExtensions[i];
    }

    return extensions;
}

bool FileFormat::contains(const std::string &extension) const
{
    return std::find(myFileExtensions.begin(), myFileExtensions.end(),
                     extension) != myFileExtensions.end();
}

FileFormatImporter::FileFormatImporter(const FileFormat &format) :
    myFormat(format)
{
}

FileFormatImporter::~FileFormatImporter()
{
}

FileFormat FileFormatImporter::fileFormat() const
{
    return myFormat;
}

FileFormatException::FileFormatException(const std::string& error)
    : std::runtime_error(error)
{
}


FileFormatExporter::FileFormatExporter(const FileFormat &format)
    : myFormat(format)
{
}

FileFormatExporter::~FileFormatExporter()
{
}

FileFormat FileFormatExporter::fileFormat() const
{
    return myFormat;
}
