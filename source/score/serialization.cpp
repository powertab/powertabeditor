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

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/gregorian/formatters_limited.hpp>
#include <boost/date_time/gregorian/parsers.hpp>
#include <iostream>

namespace ScoreUtils::detail
{
InputArchive::InputArchive(std::istream &is)
{
    if (!is)
        throw std::runtime_error("Could not open stream");

    is >> myDocument;
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

void
InputArchive::read(Util::Date &date)
{
    std::string date_str;
    read(date_str);
    auto greg_date = boost::gregorian::from_undelimited_string(date_str);
    date = Util::Date(greg_date.year(), greg_date.month(), greg_date.day());
}

JSONValue
OutputArchive::convert(const Util::Date &date)
{
    return convert(boost::gregorian::to_iso_string(
        boost::gregorian::date(date.year(), date.month(), date.day())));
}
}
