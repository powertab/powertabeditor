/*
 * Copyright (C) 2022 Cameron White
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

#include "gp7exporter.h"

#include "from_pt2.h"
#include "gp7importer.h"
#include "document.h"

#include <memory>
#include <minizip/zip.h>
#ifdef _WIN32
#include <minizip/iowin32.h>
#endif

#include <pugixml.hpp>

Gp7Exporter::Gp7Exporter()
    : FileFormatExporter(Gp7Importer::getFileFormat())
{
}

namespace
{
/// Custom deleter for use with std::unique_ptr.
struct ZipFileCloser
{
    using pointer = zipFile;

    void operator()(zipFile f)
    {
        if (f)
            zipClose(f, nullptr);
    }
};

/// Handle to a zipFile. Calls zipClose() when it goes out of scope.
using ZipFileHandle = std::unique_ptr<zipFile, ZipFileCloser>;

/// Opens a zip file for writing.
static ZipFileHandle
createZipFile(const std::filesystem::path &filename)
{
    zlib_filefunc64_def ffunc;
#ifdef _WIN32
    // Ensure wide strings are used, matching boost::filesystem::path.
    fill_win32_filefunc64W(&ffunc);
#else
    fill_fopen64_filefunc(&ffunc);
#endif

    ZipFileHandle zip_file;
    zip_file.reset(zipOpen2_64(filename.c_str(), 0, nullptr, &ffunc));
    if (!zip_file)
        throw FileFormatException("Failed to create zip file.");

    return zip_file;
}

/// RAII wrapper for creating a file in the archive.
struct ZipFileEntry
{
    ZipFileEntry(zipFile file, const char *filename) : myFile(file)
    {
        if (zipOpenNewFileInZip64(myFile, filename, nullptr, nullptr, 0,
                                  nullptr, 0, nullptr, Z_DEFLATED,
                                  Z_DEFAULT_COMPRESSION, 1) != ZIP_OK)
        {
            throw FileFormatException("Failed to create file entry.");
        }
    }

    ~ZipFileEntry()
    {
        [[maybe_unused]] const int ret = zipCloseFileInZip(myFile);
        assert(ret == ZIP_OK);
    }

    zipFile myFile;
};

static void
writeVersionInfo(zipFile file)
{
    ZipFileEntry entry(file, "VERSION");

    const std::string contents = "7.0";
    zipWriteInFileInZip(file, contents.data(), contents.size());
}

/// pugi::xml_writer implementation for writing chunks to a zip entry.
struct zip_entry_writer : public pugi::xml_writer
{
    zip_entry_writer(zipFile file) : myFile(file)
    {
    }

    void write(const void *data, size_t size) override
    {
        zipWriteInFileInZip(myFile, data, size);
    }

    zipFile myFile;
};

static void
writeScore(zipFile file, const Score &score)
{
    ZipFileEntry entry(file, "Content/score.gpif");

    Gp7::Document doc = Gp7::convert(score);
    pugi::xml_document xml_doc = Gp7::to_xml(doc);

    zip_entry_writer writer(file);
    xml_doc.save(writer);
}

} // namespace

void
Gp7Exporter::save(const std::filesystem::path &filename, const Score &score)
{
    // The .gp file format is just a zip file with a different extension.
    ZipFileHandle zip_file = createZipFile(filename);

    writeVersionInfo(zip_file.get());
    writeScore(zip_file.get(), score);
}
