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
