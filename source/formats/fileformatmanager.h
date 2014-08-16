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
  
#ifndef FORMATS_FILEFORMATMANAGER_H
#define FORMATS_FILEFORMATMANAGER_H

#include <boost/optional/optional.hpp>
#include <map>
#include <memory>
#include "fileformat.h"

class FileFormatImporter;
class FileFormatExporter;
class QWidget;
class Score;

/// An interface for import/export of various file formats.
class FileFormatManager
{
public:
    FileFormatManager();

    /// Returns the file format corresponding to the given extension.
    boost::optional<FileFormat> findFormat(const std::string &extension) const;

    /// Returns a correctly formatted file filter for a Qt file dialog.
    /// e.g. "FileType (*.ext1 *.ext2);;FileType2 (*.ext3)".
    std::string importFileFilter() const;

    /// Imports a file into the given score.
    bool importFile(Score &score, const std::string &filename,
                    const FileFormat &format, QWidget *parentWindow);

    /// Returns a correctly formatted file filter for a Qt file dialog.
    std::string exportFileFilter() const;

    /// Exports the given score to a file.
    bool exportFile(const Score &score, const std::string &filename,
                    const FileFormat &format);

private:
    template <typename Importer>
    void registerImporter();

    template <typename Exporter>
    void registerExporter();

    std::map<FileFormat, std::unique_ptr<FileFormatImporter>> myImporters;
    std::map<FileFormat, std::unique_ptr<FileFormatExporter>> myExporters;
};

template <typename Importer>
void FileFormatManager::registerImporter()
{
    FileFormat format = Importer().fileFormat();
    myImporters[format].reset(new Importer());
}

template <typename Exporter>
void FileFormatManager::registerExporter()
{
    FileFormat format = Exporter().fileFormat();
    myExporters[format].reset(new Exporter());
}

#endif
