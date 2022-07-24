/*
  * Copyright (C) 2022 Cameron White
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

#ifndef UTIL_TOUTF8_H
#define UTIL_TOUTF8_H

#include <algorithm>
#include <string>

namespace Util
{
/// Convert from ISO 8859-1 to UTF8
inline void
convertISO88591ToUTF8(std::string &str)
{
    const size_t count = std::count_if(
        str.begin(), str.end(), [](unsigned char c) { return c >= 0x80; });
    if (!count)
        return;

    std::string utf8;
    utf8.reserve(str.length() + count + 1);
    for (unsigned char c : str)
    {
        if (c < 0x80)
            utf8.push_back(c);
        else
        {
            utf8.push_back(0xc2 | (c >> 6));
            utf8.push_back(0x80 | (c & 0x3f));
        }
    }

    str = std::move(utf8);
}
} // namespace Util

#endif
