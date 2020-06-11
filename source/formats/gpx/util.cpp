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

uint32_t
Gpx::Util::readUInt(const std::vector<std::byte> &bytes, size_t index)
{
    const uint32_t n1 = std::to_integer<uint32_t>(bytes[index]);
    const uint32_t n2 = std::to_integer<uint32_t>(bytes[index + 1]);
    const uint32_t n3 = std::to_integer<uint32_t>(bytes[index + 2]);
    const uint32_t n4 = std::to_integer<uint32_t>(bytes[index + 3]);

    return n1 | (n2 << 8) | (n3 << 16) | (n4 << 24);
}
