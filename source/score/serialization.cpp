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

namespace ScoreUtils
{
InputArchive::InputArchive(std::istream &is) : myStream(is)
{
    if (!is)
        throw std::runtime_error("Could not open stream");

    myDocument.ParseStream<0>(myStream);

    if (myDocument.HasParseError())
    {
        throw std::runtime_error(std::string("Parse error: ") +
                                 myDocument.GetParseError());
    }

    myIterators.push(myDocument.MemberBegin());

    (*this)("version", myVersion);
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
