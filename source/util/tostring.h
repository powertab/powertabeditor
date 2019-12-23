/*
  * Copyright (C) 2019 Cameron White
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

#ifndef UTIL_TOSTRING_H
#define UTIL_TOSTRING_H

#include <string>
#include <sstream>

namespace Util
{
template <typename T>
std::string toString(const T &val)
{
    std::ostringstream stream;
    stream << val;
    return stream.str();
}
} // namespace Util

#endif
