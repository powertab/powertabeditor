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
  
#ifndef IFILEFORMAT_H
#define IFILEFORMAT_H

#include <string>
#include <memory>
#include <stdexcept>

class PowerTabDocument;

struct FileFormat
{
    FileFormat(const std::string& name, const std::string& fileExtensions);
    bool operator<(const FileFormat& format) const;

    std::string name; ///< Name of the file format (e.g. "Power Tab", "MIDI", "PDF", etc)
    std::string fileExtensions; ///< Supported file extensions, separated by spaces (e.g. "*.cpp *.h")
};

/// Base class for all file format importers
class FileFormatImporter
{
public:
    FileFormatImporter(const FileFormat& format);
    virtual ~FileFormatImporter();

    /// Converts the given file into a PowerTabDocument
    /// @throw FileFormatException
    virtual std::shared_ptr<PowerTabDocument> load(const std::string& fileName) = 0;

    FileFormat getFileFormat() const;

private:
    const FileFormat format;
};

/// Base class for all file format exporters
class FileFormatExporter
{
public:
    FileFormatExporter(const FileFormat& format);
    virtual ~FileFormatExporter();

    /// Converts the given file into a PowerTabDocument
    /// @throw FileFormatException
    virtual void save(std::shared_ptr<const PowerTabDocument>, const std::string& fileName) = 0;

    FileFormat getFileFormat() const;

private:
    const FileFormat format;
};

/// Exception used for any errors with format conversions (e.g. corrupted file, unsupported version, etc)
class FileFormatException : public std::runtime_error
{
public:
    FileFormatException(const std::string& error);
};

#endif // IFILEFORMAT_H
