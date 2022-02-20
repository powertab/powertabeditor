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

#include "fileformatmanager.h"

#include <formats/gp7/gp7exporter.h>
#include <formats/gp7/gp7importer.h>
#include <formats/gpx/gpximporter.h>
#include <formats/guitar_pro/guitarproimporter.h>
#include <formats/midi/midiexporter.h>
#include <formats/powertab/powertabexporter.h>
#include <formats/powertab/powertabimporter.h>
#include <formats/powertab_old/powertaboldimporter.h>

FileFormatManager::FileFormatManager(const SettingsManager &settings_manager)
{
    myImporters.emplace_back(new PowerTabImporter());
    myImporters.emplace_back(new PowerTabOldImporter());
    myImporters.emplace_back(new GuitarProImporter());
    myImporters.emplace_back(new GpxImporter());
    myImporters.emplace_back(new Gp7Importer());

    myExporters.emplace_back(new PowerTabExporter());
    myExporters.emplace_back(new Gp7Exporter());
    myExporters.emplace_back(new MidiExporter(settings_manager));
}

std::optional<FileFormat> FileFormatManager::findFormat(
    const std::string &extension) const
{
    for (auto &importer : myImporters)
    {
        if (importer->fileFormat().contains(extension))
            return importer->fileFormat();
    }

    for (auto &exporter : myExporters)
    {
        if (exporter->fileFormat().contains(extension))
            return exporter->fileFormat();
    }

    return std::nullopt;
}

std::string FileFormatManager::importFileFilter() const
{
    std::string filterAll = "All Supported Formats (";
    std::string filterOther;

    for (auto it = myImporters.begin(); it != myImporters.end(); ++it)
    {
        if (it != myImporters.begin())
            filterAll += " ";

        const FileFormat &format = (*it)->fileFormat();
        filterAll += format.allExtensions();
        filterOther += ";;" + format.fileFilter();
    }

    filterAll += ")";

    return filterAll + filterOther;
}

void FileFormatManager::importFile(Score &score,
                                   const std::filesystem::path &filename,
                                   const FileFormat &format)
{
    for (auto &importer : myImporters)
    {
        if (importer->fileFormat() == format)
        {
            importer->load(filename, score);
            return;
        }
    }

    throw std::runtime_error("Unknown file format");
}

std::string FileFormatManager::exportFileFilter() const
{
    std::string filter;

    for (auto it = myExporters.begin(); it != myExporters.end(); ++it)
    {
        if (it != myExporters.begin())
            filter += ";;";

        filter += (*it)->fileFormat().fileFilter();
    }

    return filter;
}

void FileFormatManager::exportFile(const Score &score,
                                   const std::filesystem::path &filename,
                                   const FileFormat &format)
{
    for (auto &exporter : myExporters)
    {
        if (exporter->fileFormat() == format)
        {
            exporter->save(filename, score);
            return;
        }
    }

    throw std::runtime_error("Unknown file format");
}

bool
FileFormatManager::extensionImportSupported(const std::string &extension) const
{
    for (auto const &importer : myImporters)
        if (importer->fileFormat().contains(extension))
            return true;
    return false;
}

std::vector<FileFormat>
FileFormatManager::exportFormats() const
{
    std::vector<FileFormat> formats;
    for (auto &&exporter : myExporters)
        formats.push_back(exporter->fileFormat());

    return formats;
}
