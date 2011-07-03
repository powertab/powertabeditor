#include "fileformatmanager.h"

#include <boost/foreach.hpp>

#include <QMessageBox>

#include <formats/guitar_pro/guitarproimporter.h>

FileFormatManager::FileFormatManager()
{
    FileFormat format = GuitarProImporter().getFileFormat();
    importers.insert(format, new GuitarProImporter());
}

FileFormatManager::~FileFormatManager()
{
}

/// Returns a list of all supported file formats for importing
std::vector<FileFormat> FileFormatManager::importedFileFormats() const
{
    std::vector<FileFormat> formats;

    BOOST_FOREACH(auto importer, importers)
    {
        formats.push_back(importer->first);
    }

    return formats;
}

std::shared_ptr<PowerTabDocument> FileFormatManager::import(const std::string& fileName,
                                                            const FileFormat& format)
{
    if (importers.find(format) != importers.end())
    {
        try
        {
            return importers.at(format).load(fileName);
        }
        catch(const std::exception& e)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Error importing file - ") + QString(e.what()));
            msgBox.exec();
        }
    }

    return std::shared_ptr<PowerTabDocument>();
}
