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

#include <formats/gpx/gpximporter.h>
#include <formats/guitar_pro/guitarproimporter.h>
#include <formats/midi/midiexporter.h>
#include <formats/powertab/powertabimporter.h>
#include <formats/powertab/powertabexporter.h>
#include <formats/powertab_old/powertaboldimporter.h>
#include <QMessageBox>

FileFormatManager::FileFormatManager()
{
    myImporters.emplace_back(new PowerTabImporter());
    myImporters.emplace_back(new PowerTabOldImporter());
    myImporters.emplace_back(new GuitarProImporter());
    myImporters.emplace_back(new GpxImporter());

    myExporters.emplace_back(new PowerTabExporter());
    myExporters.emplace_back(new MidiExporter());
}

boost::optional<FileFormat> FileFormatManager::findFormat(
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

    return boost::none;
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

bool FileFormatManager::importFile(Score &score, const std::string &filename,
                                   const FileFormat &format,
                                   QWidget *parent_window)
{
    for (auto &importer : myImporters)
    {
        if (importer->fileFormat() == format)
        {
            try
            {
                importer->load(filename, score);
                return true;
            }
            catch (const std::exception &e)
            {
                QMessageBox msg(parent_window);
                msg.setText(QObject::tr("Error importing file - ") +
                            QString(e.what()));
                msg.exec();
                return false;
            }
        }
    }

    return false;
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

bool FileFormatManager::exportFile(const Score &score,
                                   const std::string &filename,
                                   const FileFormat &format,
                                   QWidget *parent_window)
{
    for (auto &exporter : myExporters)
    {
        if (exporter->fileFormat() == format)
        {
            try
            {
                exporter->save(filename, score);
                return true;
            }
            catch (const std::exception &e)
            {
                QMessageBox msg(parent_window);
                msg.setText(QObject::tr("Error saving file - ") +
                            QString(e.what()));
                msg.exec();
                return false;
            }
        }
    }

    return false;
}
