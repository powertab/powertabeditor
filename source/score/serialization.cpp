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

#include "serialization.h"

#include <iostream>
#include <rapidjson/error/en.h>

namespace ScoreUtils
{
InputArchive::InputArchive(std::istream &is) : myStream(is)
{
    if (!is)
        throw std::runtime_error("Could not open stream");

    myDocument.ParseStream<0>(myStream);

    if (myDocument.HasParseError())
    {
        throw std::runtime_error("Parse error at offset " +
                                 std::to_string(myDocument.GetErrorOffset()) +
                                 ": " +
                                 GetParseError_En(myDocument.GetParseError()));
    }

    myValueStack.push(&myDocument);

    int version = 0;
    (*this)("version", version);

    if (version >= static_cast<int>(FileVersion::INITIAL_VERSION) &&
        version <= static_cast<int>(FileVersion::LATEST_VERSION))
    {
        myVersion = static_cast<FileVersion>(version);
    }
    else
    {
        std::cerr << "Warning: Reading an unknown file version - " << version
                  << std::endl;

        // Reading in a newer version. Just do the best we can with the latest
        // file version we're aware of.
        myVersion = FileVersion::LATEST_VERSION;
    }
}

FileVersion InputArchive::version() const
{
    return myVersion;
}

OutputArchive::OutputArchive(std::ostream &os, FileVersion version)
    : myWriteStream(os), myStream(myWriteStream), myVersion(version)
{
    myStream.StartObject();

    (*this)("version", myVersion);
}

OutputArchive::~OutputArchive()
{
    myStream.EndObject();
}
}
