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
  
#include "rect.h"

namespace PowerTabDocument {

Rect::Rect(void)
{
    x_ = y_ = width_ = height_ = 0;
}

Rect::Rect(int32_t x, int32_t y, int32_t width, int32_t height)
{
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
}

bool Rect::operator==(const Rect& rhs) const
{
    return (x_ == rhs.x_ &&
            y_ == rhs.y_ &&
            width_ == rhs.width_ &&
            height_ == rhs.height_);
}

int32_t Rect::GetLeft() const
{
    return GetX();
}

void Rect::SetLeft(int32_t x)
{
    SetX(x);
}

int32_t Rect::GetX() const
{
    return x_;
}

void Rect::SetX(int32_t x)
{
    x_ = x;
}

int32_t Rect::GetTop() const
{
    return GetY();
}

void Rect::SetTop(int32_t top)
{
    SetY(top);
}

int32_t Rect::GetY() const
{
    return y_;
}

void Rect::SetY(int32_t y)
{
    y_ = y;
}

int32_t Rect::GetBottom() const
{
    return y_ + height_ - 1;
}

void Rect::SetBottom(int32_t bottom)
{
    height_ = bottom - y_ - 1;
}

int32_t Rect::GetWidth() const
{
    return width_;
}

void Rect::SetWidth(int32_t width)
{
    width_ = width;
}

int32_t Rect::GetHeight() const
{
    return height_;
}

void Rect::SetHeight(int32_t height)
{
    height_ = height;
}

int32_t Rect::GetRight() const
{
    return x_ + width_ - 1;
}

void Rect::SetRight(int32_t right)
{
    width_ = right - x_ - 1;
}

}
