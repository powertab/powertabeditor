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

#include "dynamic.h"

#include <util/enumtostring.h>

Dynamic::Dynamic()
    : myPosition(0),
      myVolume(VolumeLevel::fff)
{
}

Dynamic::Dynamic(int position, VolumeLevel level)
    : myPosition(position),
      myVolume(level)
{
}

bool Dynamic::operator==(const Dynamic &other) const
{
    return myPosition == other.myPosition &&
           myVolume == other.myVolume;
}

int Dynamic::getPosition() const
{
    return myPosition;
}

void Dynamic::setPosition(int position)
{
    myPosition = position;
}

VolumeLevel Dynamic::getVolume() const
{
    return myVolume;
}

void Dynamic::setVolume(VolumeLevel level)
{
    myVolume = level;
}

UTIL_DEFINE_ENUMTOSTRING(VolumeLevel, {
    { VolumeLevel::Off, "Off" },
    { VolumeLevel::ppp, "ppp" },
    { VolumeLevel::pp, "pp" },
    { VolumeLevel::p, "p" },
    { VolumeLevel::mp, "mp" },
    { VolumeLevel::mf, "mf" },
    { VolumeLevel::f, "f" },
    { VolumeLevel::ff, "ff" },
    { VolumeLevel::fff, "fff" },
})
