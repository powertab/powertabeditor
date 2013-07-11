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

#include "position.h"

namespace Score {

Position::Position()
    : myPosition(0),
      myDurationType(EighthNote),
      myIrregularGroupTiming(1, 1)
{
}

Position::Position(int position, DurationType duration)
    : myPosition(position),
      myDurationType(duration),
      myIrregularGroupTiming(1, 1)
{
}

bool Position::operator==(const Position &other) const
{
    return myPosition == other.myPosition &&
           myDurationType == other.myDurationType &&
           mySimpleProperties == other.mySimpleProperties &&
           myIrregularGroupTiming == other.myIrregularGroupTiming;
}

int Position::getPosition() const
{
    return myPosition;
}

void Position::setPosition(int position)
{
    myPosition = position;
}

Position::DurationType Position::getDurationType() const
{
    return myDurationType;
}

void Position::setDurationType(DurationType type)
{
    myDurationType = type;
}

bool Position::hasProperty(SimpleProperty property) const
{
    return mySimpleProperties.test(property);
}

void Position::setProperty(SimpleProperty property, bool set)
{
    mySimpleProperties.set(property, set);
}

bool Position::isRest() const
{
    return hasProperty(Rest);
}

void Position::setRest(bool set)
{
    setProperty(Rest, set);
}

}
