/*
 * Copyright (C) 2020 Cameron White
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

#include "gp7importer.h"

#include "converter.h"
#include "parser.h"

#include <minizip/unzip.h>
#ifdef _WIN32
#include <minizip/iowin32.h>
#endif

#include <pugixml.hpp>

#include <formats/fileformat.h>
#include <score/score.h>
#include <util/scopeexit.h>

Gp7Importer::Gp7Importer()
    : FileFormatImporter(FileFormat("Guitar Pro 7", { "gp" }))
{
}

namespace
{
/// Custom deleter for use with std::unique_ptr.
struct UnzFileCloser
{
    using pointer = unzFile;

    void operator()(unzFile f)
    {
        if (f)
            unzClose(f);
    }
};

/// Handle to a unzFile. Calls unzClose() when it goes out of scope.
using UnzFileHandle = std::unique_ptr<unzFile, UnzFileCloser>;

/// Opens a zip file.
UnzFileHandle openZipFile(const std::filesystem::path &filename)
{
    zlib_filefunc64_def ffunc;
#ifdef _WIN32
    // Ensure wide strings are used, matching boost::filesystem::path.
    fill_win32_filefunc64W(&ffunc);
#else
    fill_fopen64_filefunc(&ffunc);
#endif

    UnzFileHandle zip_file;
    zip_file.reset(unzOpen2_64(filename.c_str(), &ffunc));
    if (!zip_file)
        throw FileFormatException("Failed to unzip file.");

    return zip_file;
}

/// Loads a file from the provided zip archive.
std::vector<std::byte> loadFileFromZip(unzFile zip_file, const char *name)
{
    if (unzLocateFile(zip_file, name, 1) == UNZ_END_OF_LIST_OF_FILE)
        throw FileFormatException("Could not find file in archive.");

    if (unzOpenCurrentFile(zip_file) != UNZ_OK)
        throw FileFormatException("Failed to open file in archive.");

    // Close the file upon going out of scope.
    Util::ScopeExit close_file([&]() {
        if (unzCloseCurrentFile(zip_file) != UNZ_OK)
            throw FileFormatException("Failed to close file.");
    });

    // Read the file contents, block by block.
    std::vector<std::byte> buffer;
    while (true)
    {
        static constexpr int BLOCK_SIZE = 4096;

        const size_t block_start = buffer.size();
        buffer.resize(buffer.size() + BLOCK_SIZE);
        const int bytes_read = unzReadCurrentFile(
            zip_file, buffer.data() + block_start, BLOCK_SIZE);

        if (bytes_read == 0)
        {
            // End of file.
            break;
        }
        else if (bytes_read < 0)
        {
            throw FileFormatException("Failed to read file.");
        }
        else if (bytes_read != BLOCK_SIZE)
        {
            // We likely reached the end of the file - trim extra entries that
            // were allocated.
            assert(bytes_read < BLOCK_SIZE);
            buffer.resize(buffer.size() - (BLOCK_SIZE - bytes_read));
        }
    };

    return buffer;
}

} // namespace

void Gp7Importer::load(const std::filesystem::path &filename, Score &score)
{
    // The .gp file format is just a zip file with a different extension.
    UnzFileHandle zip_file = openZipFile(filename);

    // There are a few files, but Content/score.gpif has the main contents in
    // XML format. This is very similar to the .gpx file format, but with a
    // different container.
    std::vector<std::byte> buffer =
        loadFileFromZip(zip_file.get(), "Content/score.gpif");

    // Parse as an XML file.
    pugi::xml_document xml_doc;
    pugi::xml_parse_result result =
        xml_doc.load_buffer_inplace(buffer.data(), buffer.size());
    if (!result)
        throw FileFormatException(result.description());

    Gp7::Document doc = Gp7::parse(xml_doc, Gp7::Version::V7);
    Gp7::convert(doc, score);
}
