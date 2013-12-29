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
  
#ifndef HARMONICS_H
#define HARMONICS_H

#include <cstdint>
#include <vector>

/// Contains useful utility functions for working with
/// natural harmonics and tapped harmonics that fall outside
/// of the scope of the Note class
namespace Harmonics
{
    uint8_t getPitchOffset(uint8_t fretOffset);
    std::vector<uint8_t> getFretOffsets();
}

#endif // HARMONICS_H
