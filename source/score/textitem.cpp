/*
  * Copyright (C) 2014 Cameron White
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

#include "textitem.h"

TextItem::TextItem() : myPosition(0)
{
}

TextItem::TextItem(int position, const std::string &contents)
    : myPosition(position), myContents(contents)
{
}

bool TextItem::operator==(const TextItem &other) const
{
    return myPosition == other.myPosition && myContents == other.myContents;
}

int TextItem::getPosition() const
{
    return myPosition;
}

void TextItem::setPosition(int position)
{
    myPosition = position;
}

const std::string &TextItem::getContents() const
{
    return myContents;
}

void TextItem::setContents(const std::string &contents)
{
    myContents = contents;
}
