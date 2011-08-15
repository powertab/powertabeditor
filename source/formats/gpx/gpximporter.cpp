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
#include "documentreader.h"
#include <fstream>
#include <boost/make_shared.hpp>

#include <powertabdocument/powertabdocument.h>

GpxImporter::GpxImporter() :
    FileFormatImporter(FileFormat("Guitar Pro 6", "*.gpx"))
{
}

boost::shared_ptr<PowerTabDocument> GpxImporter::load(const std::string& fileName)
{
    // load the data, decompress, and open as XML document
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    Gpx::FileSystem fileSystem(Gpx::load(file));

    const std::string scoreData= fileSystem.getFileContents("score.gpif");

    boost::shared_ptr<PowerTabDocument> doc = boost::make_shared<PowerTabDocument>();
    Gpx::DocumentReader reader(scoreData);
    reader.readDocument(doc);

    return doc;
}
