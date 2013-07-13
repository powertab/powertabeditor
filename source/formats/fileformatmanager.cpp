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

#include <formats/powertab/powertabimporter.h>
#include <formats/powertab/powertabexporter.h>
#include <formats/powertab_old/powertaboldimporter.h>
#include <QMessageBox>

FileFormatManager::FileFormatManager()
{
    registerImporter<PowerTabImporter>();
    registerImporter<PowerTabOldImporter>();
    registerExporter<PowerTabExporter>();
}

boost::optional<FileFormat> FileFormatManager::findFormat(
        const std::string &extension) const
{
    for (ImporterMap::const_iterator importer = myImporters.begin();
         importer != myImporters.end(); ++importer)
    {
        if (importer->first.contains(extension))
            return importer->first;
    }

    return 0;
}

std::string FileFormatManager::importFileFilter() const
{
    std::string filterAll = "All Supported Formats (";
    std::string filterOther;

    for (ImporterMap::const_iterator importer = myImporters.begin();
         importer != myImporters.end(); ++importer)
    {
        if (importer != myImporters.begin())
            filterAll += " ";

        filterAll += importer->first.allExtensions();
        filterOther += ";;" + importer->first.fileFilter();
    }

    filterAll += ")";

    return filterAll + filterOther;
}

bool FileFormatManager::importFile(Score &score, const std::string &filename,
                                   const FileFormat &format, QWidget *parentWindow)
{
    if (myImporters.find(format) != myImporters.end())
    {
        try
        {
            myImporters.at(format).load(filename, score);
            return true;
        }
        catch (const std::exception &e)
        {
            QMessageBox msgBox(parentWindow);
            msgBox.setText(QObject::tr("Error importing file - ") + QString(e.what()));
            msgBox.exec();
            return false;
        }
    }

    return false;
}

std::string FileFormatManager::exportFileFilter() const
{
    std::string filter;

    for (ExporterMap::const_iterator exporter = myExporters.begin();
         exporter != myExporters.end(); ++exporter)
    {
        if (exporter != myExporters.begin())
            filter += ";;";

        filter += exporter->first.fileFilter();
    }

    return filter;
}

bool FileFormatManager::exportFile(const Score &score, const std::string &filename,
                                   const FileFormat &format)
{
    if (myExporters.find(format) != myExporters.end())
    {
        try
        {
            myExporters.at(format).save(filename, score);
            return true;
        }
        catch (const std::exception &e)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Error saving file - ") + QString(e.what()));
            msgBox.exec();
        }
    }

    return false;
}
