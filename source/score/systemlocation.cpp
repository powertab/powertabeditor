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
  
#include "systemlocation.h"

#include <boost/functional/hash.hpp>

SystemLocation::SystemLocation(int system, int position)
    : mySystem(system),
      myPosition(position)
{
}

SystemLocation::SystemLocation()
    : mySystem(0),
      myPosition(0)
{
}

bool SystemLocation::operator<(const SystemLocation &location) const
{
    if (mySystem == location.mySystem)
        return myPosition < location.myPosition;
    else
        return mySystem < location.mySystem;
}

bool SystemLocation::operator==(const SystemLocation &location) const
{
    return mySystem == location.mySystem && myPosition == location.myPosition;
}

void SystemLocation::setPosition(int position)
{
    myPosition = position;
}

int SystemLocation::getPosition() const
{
    return myPosition;
}

void SystemLocation::setSystem(int system)
{
    mySystem = system;
}

int SystemLocation::getSystem() const
{
    return mySystem;
}

size_t std::hash<SystemLocation>::operator()(
    const SystemLocation &location) const
{
    size_t seed = 0;
    boost::hash_combine(seed, location.getSystem());
    boost::hash_combine(seed, location.getPosition());
    return seed;
}
