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

#include <boost/foreach.hpp>

#include <QMessageBox>

#include <formats/guitar_pro/guitarproimporter.h>
#include <formats/gpx/gpximporter.h>
#include <formats/powertab/powertabimporter.h>
#include <formats/powertab/powertabexporter.h>
#include <powertabdocument/powertabdocument.h>

FileFormatManager::FileFormatManager()
{
    registerImporter<PowerTabImporter>();
    registerImporter<GuitarProImporter>();
    registerImporter<GpxImporter>();

    registerExporter<PowerTabExporter>();
}

FileFormatManager::~FileFormatManager()
{
}

boost::optional<FileFormat> FileFormatManager::findFormat(
        const std::string &extension) const
{
    for (boost::ptr_map<FileFormat, FileFormatImporter>::const_iterator importer = importers.begin();
         importer != importers.end(); ++importer)
    {
        if (importer->first.contains(extension))
            return importer->first;
    }

    return 0;
}

/// Returns a correctly formatted file filter for a Qt file dialog.
/// e.g. "FileType (*.ext1 *.ext2);;FileType2 (*.ext3)".
std::string FileFormatManager::importFileFilter() const
{
    std::string filterAll = "All Supported Formats (";
    std::string filterOther;

    for (boost::ptr_map<FileFormat, FileFormatImporter>::const_iterator importer = importers.begin();
         importer != importers.end(); ++importer)
    {
        if (importer != importers.begin())
            filterAll += " ";

        filterAll += importer->first.allExtensions();
        filterOther += ";;" + importer->first.fileFilter();
    }

    filterAll += ")";

    return filterAll + filterOther;
}

boost::shared_ptr<PowerTabDocument> FileFormatManager::importFile(
        const std::string& fileName, const FileFormat& format)
{
    if (importers.find(format) != importers.end())
    {
        try
        {
            boost::shared_ptr<PowerTabDocument> doc(importers.at(format).load(fileName));
            doc->SetFileName(fileName);
            return doc;
        }
        catch(const std::exception& e)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Error importing file - ") + QString(e.what()));
            msgBox.exec();
        }
    }

    return boost::shared_ptr<PowerTabDocument>();
}

std::string FileFormatManager::exportFileFilter() const
{
    std::string filter;

    for (boost::ptr_map<FileFormat, FileFormatExporter>::const_iterator exporter = exporters.begin();
         exporter != exporters.end(); ++exporter)
    {
        if (exporter != exporters.begin())
            filter += ";;";

        filter += exporter->first.fileFilter();
    }

    return filter;
}

bool FileFormatManager::exportFile(
        boost::shared_ptr<const PowerTabDocument> doc,
        const std::string &fileName, const FileFormat &format)
{
    if (exporters.find(format) != exporters.end())
    {
        try
        {
            exporters.at(format).save(doc, fileName);
            return true;
        }
        catch(const std::exception& e)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Error saving file - ") + QString(e.what()));
            msgBox.exec();
        }
    }

    return false;
}
