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

/// Converts 4 bytes starting at the given index into an integer.
uint32_t Gpx::Util::readUInt(const std::vector<uint8_t>& bytes, size_t index)
{
    const uint32_t n1 = bytes[index];
    const uint32_t n2 = bytes[index + 1];
    const uint32_t n3 = bytes[index + 2];
    const uint32_t n4 = bytes[index + 3];

    return n1 | (n2 << 8) | (n3 << 16) | (n4 << 24);
}
