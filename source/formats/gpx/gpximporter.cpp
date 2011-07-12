#include "gpximporter.h"

#include "filesystem.h"
#include "documentreader.h"
#include <fstream>

#include <powertabdocument/powertabdocument.h>

GpxImporter::GpxImporter() :
    FileFormatImporter(FileFormat("Guitar Pro 6", "*.gpx"))
{
}

std::shared_ptr<PowerTabDocument> GpxImporter::load(const std::string& fileName)
{
    // load the data, decompress, and open as XML document
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    Gpx::FileSystem fileSystem(Gpx::load(file));

    const std::string scoreData= fileSystem.getFileContents("score.gpif");

    auto doc = std::make_shared<PowerTabDocument>();
    Gpx::DocumentReader reader(scoreData);
    reader.readDocument(doc);

    return doc;
}
