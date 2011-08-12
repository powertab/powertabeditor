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
  
#include "systemlocation.h"

#include <boost/functional/hash.hpp>

SystemLocation::SystemLocation(uint32_t system, uint32_t position) :
    system(system),
    position(position)
{
}

SystemLocation::SystemLocation() :
    system(0),
    position(0)
{
}

bool SystemLocation::operator<(const SystemLocation& location) const
{
    if (system == location.system)
    {
        return position < location.position;
    }
    else
    {
        return system < location.system;
    }
}

bool SystemLocation::operator==(const SystemLocation& location) const
{
    return system == location.system && position == location.position;
}

void SystemLocation::setPositionIndex(uint32_t position)
{
    this->position = position;
}

uint32_t SystemLocation::getPositionIndex() const
{
    return position;
}

void SystemLocation::setSystemIndex(uint32_t system)
{
    this->system = system;
}

uint32_t SystemLocation::getSystemIndex() const
{
    return system;
}

/// Enable the use of SystemLocation as a key for boost::unorded_map, etc
size_t hash_value(const SystemLocation& location)
{
    size_t seed = 0;
    boost::hash_combine(seed, location.getSystemIndex());
    boost::hash_combine(seed, location.getPositionIndex());
    return seed;
}
