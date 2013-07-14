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

#include "scorelocation.h"

ScoreLocation::ScoreLocation(int system, int staff, int position)
    : mySystem(system),
      myStaff(staff),
      myPosition(position)
{
}

int ScoreLocation::getPosition() const
{
    return myPosition;
}

void ScoreLocation::setPosition(int position)
{
    myPosition = position;
}

int ScoreLocation::getStaff() const
{
    return myStaff;
}

void ScoreLocation::setStaff(int staff)
{
    myStaff = staff;
}

int ScoreLocation::getSystem() const
{
    return mySystem;
}

void ScoreLocation::setSystem(int system)
{
    mySystem = system;
}
