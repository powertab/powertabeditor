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
  
#include "colour.h"

namespace PowerTabDocument {

Colour::Colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    Set(red, green, blue, alpha);
}

void Colour::Set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    red_ = red;
    green_ = green;
    blue_ = blue;
    alpha_ = alpha;
}

bool Colour::operator==(const Colour& rhs) const
{
    return (red_ == rhs.red_ &&
            green_ == rhs.green_ &&
            blue_ == rhs.blue_ &&
            alpha_ == rhs.alpha_
            );
}

uint8_t Colour::Red() const
{
    return red_;
}

uint8_t Colour::Green() const
{
    return green_;
}

uint8_t Colour::Blue() const
{
    return blue_;
}

uint8_t Colour::Alpha() const
{
    return alpha_;
}

}
