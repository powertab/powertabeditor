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

#include "powertabexporter.h"

#include "common.h"
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <score/score.h>
#include <score/serialization.h>

PowerTabExporter::PowerTabExporter()
    : FileFormatExporter(getPowerTabFileFormat())
{
}

void PowerTabExporter::save(const std::filesystem::path &filename,
                            const Score &score)
{
    // Use gzip to compress the resulting data.
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    boost::iostreams::filtering_ostreambuf out;
    out.push(boost::iostreams::gzip_compressor());
    out.push(file);

    std::ostream compressed_output(&out);
    ScoreUtils::save(compressed_output, "score", score, /* pretty */ false);
}
