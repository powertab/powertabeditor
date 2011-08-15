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
  
#include "util.h"

#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_binary.hpp>

/// Converts 4 bytes starting at the given index into an integer
uint32_t Gpx::Util::readUInt(const std::vector<char>& bytes, size_t index)
{
    uint32_t value = 0;

    std::vector<char>::const_iterator begin = bytes.begin() + index;

    using namespace boost::spirit;
    qi::parse(begin, begin + sizeof(uint32_t), qi::dword, value);

    return value;
}
