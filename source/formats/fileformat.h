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

#include <stdexcept>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class PowerTabDocument;

class FileFormat
{
public:
    FileFormat(const std::string& name,
               const std::vector<std::string>& fileExtensions);

    bool operator<(const FileFormat& format) const;

    std::string fileFilter() const;
    std::string allExtensions() const;

    bool contains(const std::string &extension) const;

private:
    /// Name of the file format (e.g. "Power Tab", "MIDI", etc).
    std::string name;
    /// Supported file extensions (e.g. {"gp3", "gp4"}).
    std::vector<std::string> fileExtensions;
};

/// Base class for all file format importers
class FileFormatImporter
{
public:
    FileFormatImporter(const FileFormat& format);
    virtual ~FileFormatImporter();

    /// Converts the given file into a PowerTabDocument
    /// @throw FileFormatException
    virtual boost::shared_ptr<PowerTabDocument> load(const std::string& fileName) = 0;

    FileFormat fileFormat() const;

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
    virtual void save(boost::shared_ptr<const PowerTabDocument>, const std::string& fileName) = 0;

    FileFormat fileFormat() const;

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
