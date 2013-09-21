/*
 * Copyright (C) 2013 Cameron White
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

#include "chordtext.h"

ChordText::ChordText() : myPosition(0)
{
}

ChordText::ChordText(int position, const ChordName &name)
    : myPosition(position), myChordName(name)
{
}

bool ChordText::operator==(const ChordText &other) const
{
    return myPosition == other.myPosition && myChordName == other.myChordName;
}

int ChordText::getPosition() const
{
    return myPosition;
}

void ChordText::setPosition(int position)
{
    myPosition = position;
}

const ChordName &ChordText::getChordName() const
{
    return myChordName;
}

void ChordText::setChordName(const ChordName &name)
{
    myChordName = name;
}
