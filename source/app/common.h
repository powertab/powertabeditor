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
  
#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <cmath>

namespace Common {

/// Returns the value, brought into the range [low, high]
template <typename T>
inline T clamp (const T& value, const T& low, const T& high)
{
    return std::max(std::min(value, high), low);
}

/// Simple implementation of log2 for compilers with C99 support (e.g. MSVC)
inline double log2(double n)
{
	return std::log(n) / std::log(2.0);
}

}

#endif // COMMON_H
