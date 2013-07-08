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

#include "barline.h"

namespace Score {

Barline::Barline()
    : myBarType(SingleBar),
      myRepeatCount(0),
      myPosition(0)
{
}

bool Barline::operator==(const Barline &other) const
{
    return myBarType == other.myBarType &&
           myRepeatCount == other.myRepeatCount &&
           myPosition == other.myPosition &&
           myKeySignature == other.myKeySignature &&
           myTimeSignature == other.myTimeSignature &&
           myRehearsalSign == other.myRehearsalSign;
}

Barline::BarType Barline::getBarType() const
{
    return myBarType;
}

void Barline::setBarType(BarType type)
{
    myBarType = type;
}

int Barline::getRepeatCount() const
{
    return myRepeatCount;
}

void Barline::setRepeatCount(int count)
{
    myRepeatCount = count;
}

int Barline::getPosition() const
{
    return myPosition;
}

void Barline::setPosition(int position)
{
    myPosition = position;
}

const KeySignature &Barline::getKeySignature() const
{
    return myKeySignature;
}

void Barline::setKeySignature(const KeySignature &key)
{
    myKeySignature = key;
}

const TimeSignature &Barline::getTimeSignature() const
{
    return myTimeSignature;
}

void Barline::setTimeSignature(const TimeSignature &time)
{
    myTimeSignature = time;
}

const boost::optional<RehearsalSign> &Barline::getRehearsalSign() const
{
    return myRehearsalSign;
}

void Barline::setRehearsalSign(const boost::optional<RehearsalSign> &sign)
{
    myRehearsalSign = sign;
}

}

