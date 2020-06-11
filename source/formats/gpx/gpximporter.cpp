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

#include "gpximporter.h"

#include "filesystem.h"

#include <formats/gp7/parser.h>
#include <formats/gp7/converter.h>
#include <score/score.h>
#include <score/utils/scorepolisher.h>

#include <boost/filesystem/fstream.hpp>
#include <pugixml.hpp>

#include <iostream>

GpxImporter::GpxImporter()
    : FileFormatImporter(FileFormat("Guitar Pro 6", { "gpx" }))
{
}

void
GpxImporter::load(const boost::filesystem::path &filename, Score &score)
{
    // Load the data, decompress, and open as XML document.
    boost::filesystem::ifstream file(filename, std::ios::binary | std::ios::in);
    Gpx::FileSystem fs(file);

    std::vector<std::byte> buffer = fs.getFileContents("score.gpif");

    // Parse as an XML file.
    pugi::xml_document xml_doc;
    pugi::xml_parse_result result =
        xml_doc.load_buffer_inplace(buffer.data(), buffer.size());
    if (!result)
        throw FileFormatException(result.description());

    Gp7::Document doc = Gp7::parse(xml_doc, Gp7::Version::V6);
    Gp7::convert(doc, score);

    ScoreUtils::polishScore(score);
    ScoreUtils::addStandardFilters(score);
}
