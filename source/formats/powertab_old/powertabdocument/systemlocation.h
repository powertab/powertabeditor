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
  
#ifndef SYSTEMLOCATION_H
#define SYSTEMLOCATION_H

#include <boost/cstdint.hpp>
#include <boost/operators.hpp> // used to generate additional operators, e.g. <=, !=, etc

namespace PowerTabDocument {

/// Convenient class to represent a location in the score - contains system index and position index
class SystemLocation :
        boost::equality_comparable<SystemLocation>, // generate != from operator==
        boost::less_than_comparable<SystemLocation> // generate <=, >, >= from operator<
{
public:
    SystemLocation();
    SystemLocation(uint32_t system, uint32_t position);

    bool operator<(const SystemLocation& location) const;
    bool operator==(const SystemLocation& location) const;

    void setSystemIndex(uint32_t system);
    uint32_t getSystemIndex() const;

    void setPositionIndex(uint32_t position);
    uint32_t getPositionIndex() const;

private:
    uint32_t system;
    uint32_t position;
};

extern size_t hash_value(const SystemLocation& location);

}

#endif // SYSTEMLOCATION_H
