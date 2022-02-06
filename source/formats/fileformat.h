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
  
#ifndef FORMATS_FILEFORMAT_H
#define FORMATS_FILEFORMAT_H

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

class Score;

class FileFormat
{
public:
    FileFormat(const std::string &myName,
               const std::vector<std::string> &myFileExtensions);

    bool operator==(const FileFormat &format) const;

    /// Returns a correctly formatted file filter for a Qt file dialog.
    /// e.g. "FileType (*.ext1 *.ext2)".
    std::string fileFilter() const;

    std::string allExtensions() const;

    const std::string &primaryExtension() const { return myFileExtensions.front(); }

    bool contains(const std::string &extension) const;

private:
    /// Name of the file format (e.g. "Power Tab", "MIDI", etc).
    std::string myName;
    /// Supported file extensions (e.g. {"gp3", "gp4"}).
    std::vector<std::string> myFileExtensions;
};

/// Base class for all file format importers.
class FileFormatImporter
{
public:
    FileFormatImporter(const FileFormat &myFormat);
    virtual ~FileFormatImporter();

    /// Imports the file into the given score.
    /// @throw FileFormatException
    virtual void load(const std::filesystem::path &filename,
                      Score &score) = 0;

    /// Returns the file format corresponding to this importer.
    FileFormat fileFormat() const;

private:
    const FileFormat myFormat;
};

/// Base class for all file format exporters.
class FileFormatExporter
{
public:
    FileFormatExporter(const FileFormat &myFormat);
    virtual ~FileFormatExporter();

    /// Exports the given score to a file.
    /// @throw FileFormatException
    virtual void save(const std::filesystem::path &filename,
                      const Score &score) = 0;

    /// Returns the file format corresponding to this exporter.
    FileFormat fileFormat() const;

private:
    const FileFormat myFormat;
};

/// Exception used for any errors with format conversions
/// (e.g. corrupted file, unsupported version, etc).
class FileFormatException : public std::runtime_error
{
public:
    FileFormatException(const std::string &error);
};

#endif
