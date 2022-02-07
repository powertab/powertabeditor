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

#include "fileformat.h"

#include <memory>
#include <optional>
#include <vector>

class FileFormatImporter;
class FileFormatExporter;
class Score;
class SettingsManager;

/// An interface for import/export of various file formats.
class FileFormatManager
{
public:
    FileFormatManager(const SettingsManager &settings_manager);

    /// Returns the file format corresponding to the given extension.
    std::optional<FileFormat> findFormat(const std::string &extension) const;

    /// Returns a correctly formatted file filter for a Qt file dialog.
    /// e.g. "FileType (*.ext1 *.ext2);;FileType2 (*.ext3)".
    std::string importFileFilter() const;

    /// Imports a file into the given score.
    /// @throws std::exception
    void importFile(Score &score, const std::filesystem::path &filename,
                    const FileFormat &format);

    /// Returns a correctly formatted file filter for a Qt file dialog.
    std::string exportFileFilter() const;

    /// Exports the given score to a file.
    /// @throws std::exception
    void exportFile(const Score &score, const std::filesystem::path &filename,
                    const FileFormat &format);

    // Checks to see if there is an importer for the designated extension
    bool extensionImportSupported(const std::string& extension) const;

    /// Returns a list of the supported export file formats.
    std::vector<FileFormat> exportFormats() const;

private:
    template <typename Importer>
    void registerImporter();

    template <typename Exporter>
    void registerExporter();

    std::vector<std::unique_ptr<FileFormatImporter>> myImporters;
    std::vector<std::unique_ptr<FileFormatExporter>> myExporters;
};

#endif
