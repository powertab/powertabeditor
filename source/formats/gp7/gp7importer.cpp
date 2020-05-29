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

#include <minizip/unzip.h>
#ifdef _WIN32
#include <minizip/iowin32.h>
#endif

#include <formats/fileformat.h>
#include <score/score.h>
#include <score/utils/scorepolisher.h>
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

} // namespace

void Gp7Importer::load(const boost::filesystem::path &filename, Score &score)
{
    zlib_filefunc64_def ffunc;
#ifdef _WIN32
    // Ensure wide strings are used, matching boost::filesystem::path.
    fill_win32_filefunc64W(&ffunc);
#else
    fill_fopen64_filefunc(&ffunc);
#endif

    // The .gp file format is just a zip file with a different extension.
    UnzFileHandle zip_file;
    zip_file.reset(unzOpen2_64(filename.c_str(), &ffunc));
    if (!zip_file)
        throw FileFormatException("Failed to unzip file.");

    // There are a few files, but Content/score.gpif has the main contents in
    // XML format. This is very similar to the .gpx file format, but with a
    // different container.
    if (unzLocateFile(zip_file.get(), "Content/score.gpif", 1) ==
        UNZ_END_OF_LIST_OF_FILE)
    {
        throw FileFormatException("Could not find score.gpif.");
    }

    // Open score.gpif.
    if (unzOpenCurrentFile(zip_file.get()) != UNZ_OK)
        throw FileFormatException("Failed to open score.gpif.");

    // Close the file upon going out of scope.
    Util::ScopeExit close_file([&]() {
        if (unzCloseCurrentFile(zip_file.get()) != UNZ_OK)
            throw FileFormatException("Failed to close file.");
    });

    ScoreUtils::polishScore(score);
    ScoreUtils::addStandardFilters(score);
}
