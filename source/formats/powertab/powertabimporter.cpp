/*
  * Copyright (C) 2013 Cameron White
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

#include "powertabimporter.h"

#include "common.h"

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <fstream>
#include <score/score.h>
#include <score/serialization.h>

PowerTabImporter::PowerTabImporter()
    : FileFormatImporter(getPowerTabFileFormat())
{
}

void PowerTabImporter::load(const std::filesystem::path &filename,
                            Score &score)
{
    // The files are compressed by gzip, so we need to uncompress them before
    // loading the data.
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    boost::iostreams::filtering_istreambuf in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(file);

    std::istream compressed_input(&in);
    ScoreUtils::load(compressed_input, "score", score);
}
