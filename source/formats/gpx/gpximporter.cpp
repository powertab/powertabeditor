#include "gpximporter.h"

#include "filesystem.h"
#include <fstream>

GpxImporter::GpxImporter() :
    FileFormatImporter(FileFormat("Guitar Pro 6", "*.gpx"))
{
}

std::shared_ptr<PowerTabDocument> GpxImporter::load(const std::string& fileName)
{
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    Gpx::load(file);

    return std::shared_ptr<PowerTabDocument>();
}
