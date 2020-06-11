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

#ifndef FORMATS_GPX_FILESYSTEM_H
#define FORMATS_GPX_FILESYSTEM_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

namespace Gpx
{

/// The uncompressed *.gpx file is essentially a filesystem containing several
/// xml files.
/// This class handles the extraction of information from that filesystem.
class FileSystem
{
public:
    FileSystem(std::istream &stream);

    const std::vector<std::byte> &getFileContents(
        const std::string &filename) const;

private:
    void readUncompressedData(std::vector<std::byte> &data);

    /// Maps filenames to file contents.
    std::unordered_map<std::string, std::vector<std::byte>> myFiles;
};

} // namespace Gpx

#endif
