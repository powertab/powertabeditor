#ifndef GPXIMPORTER_H
#define GPXIMPORTER_H

#include <formats/fileformat.h>

class GpxImporter : public FileFormatImporter
{
public:
    GpxImporter();

    std::shared_ptr<PowerTabDocument> load(const std::string& fileName);
};

#endif // GPXIMPORTER_H
