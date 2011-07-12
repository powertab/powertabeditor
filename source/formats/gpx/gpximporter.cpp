#include "gpximporter.h"

#include "filesystem.h"
#include <fstream>

#include <QByteArray>
#include <QDebug>
#include <QDomDocument>

GpxImporter::GpxImporter() :
    FileFormatImporter(FileFormat("Guitar Pro 6", "*.gpx"))
{
}

std::shared_ptr<PowerTabDocument> GpxImporter::load(const std::string& fileName)
{
    // load the data, decompress, and open as XML document
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    Gpx::FileSystem fileSystem(Gpx::load(file));

    std::vector<char> scoreData = fileSystem.getFileContents("score.gpif");
    QByteArray bytes(scoreData.data(), scoreData.size());
    QDomDocument doc;

    if (!doc.setContent(bytes))
    {
        throw FileFormatException("Corrupted file?");
    }

    return std::shared_ptr<PowerTabDocument>();
}
