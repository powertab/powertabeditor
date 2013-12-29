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
  
#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>

namespace PowerTabDocument {

class Colour
{
public:
    Colour(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);

    bool operator==(const Colour& rhs) const;

    uint8_t Red() const;
    uint8_t Green() const;
    uint8_t Blue() const;
    uint8_t Alpha() const;
    
    void Set(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);

private:
    uint8_t red_, green_, blue_, alpha_;
};

}

#endif // COLOUR_H
