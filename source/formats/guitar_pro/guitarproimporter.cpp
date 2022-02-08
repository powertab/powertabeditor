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

#include "guitarproimporter.h"
#include "gp345to7converter.h"

#include <formats/gp7/document.h>
#include <formats/gp7/to_pt2.h>
#include <formats/guitar_pro/document.h>
#include <formats/guitar_pro/inputstream.h>
#include <fstream>

GuitarProImporter::GuitarProImporter()
    : FileFormatImporter(
          FileFormat("Guitar Pro 3, 4, 5", { "gp3", "gp4", "gp5" }))
{
}

void
GuitarProImporter::load(const std::filesystem::path &filename, Score &score)
{
    std::ifstream in(filename, std::ios::binary | std::ios::in);
    Gp::InputStream stream(in);

    Gp::Document document;
    document.load(stream);

    // Convert to the GP7 intermediate format, which then can be converted to
    // our score format.
    Gp7::Document gp7_doc = Gp::convertToGp7(document);
    Gp7::convert(gp7_doc, score);
}
