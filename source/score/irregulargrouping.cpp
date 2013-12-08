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

#include "irregulargrouping.h"

#include <ostream>

IrregularGrouping::IrregularGrouping()
    : myPosition(0), myLength(0), myNotesPlayed(0), myNotesPlayedOver(0)
{
}

IrregularGrouping::IrregularGrouping(int position, int length, int notesPlayed,
                                     int notesPlayedOver)
    : myPosition(position),
      myLength(length),
      myNotesPlayed(notesPlayed),
      myNotesPlayedOver(notesPlayedOver)
{
}

bool IrregularGrouping::operator==(const IrregularGrouping &other) const
{
    return myPosition == other.myPosition && myLength == other.myLength &&
           myNotesPlayed == other.myNotesPlayed &&
            myNotesPlayedOver == other.myNotesPlayedOver;
}

int IrregularGrouping::getPosition() const
{
    return myPosition;
}

void IrregularGrouping::setPosition(int position)
{
    myPosition = position;
}

int IrregularGrouping::getLength() const
{
    return myLength;
}

void IrregularGrouping::setLength(int length)
{
    myLength = length;
}

int IrregularGrouping::getNotesPlayed() const
{
    return myNotesPlayed;
}

void IrregularGrouping::setNotesPlayed(int notes)
{
    myNotesPlayed = notes;
}

int IrregularGrouping::getNotesPlayedOver() const
{
    return myNotesPlayedOver;
}

void IrregularGrouping::setNotesPlayedOver(int notes)
{
    myNotesPlayedOver = notes;
}

std::ostream &operator<<(std::ostream &os, const IrregularGrouping &group)
{
    os << group.getNotesPlayed();

    const int playedOver = group.getNotesPlayedOver();
    // Check for a power of 2.
    const bool isNormalGrouping = (playedOver & (playedOver - 1)) == 0;

    // Display the ratio if there is an irregular or non-standard ratio.
    if (!isNormalGrouping || group.getNotesPlayed() < playedOver)
        os << ":" << playedOver;

    return os;
}
