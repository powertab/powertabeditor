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
  
#ifndef SCORE_SYSTEMLOCATION_H
#define SCORE_SYSTEMLOCATION_H

#include <boost/operators.hpp>
#include <memory>

/// Convenience class to represent a location in a system.
class SystemLocation :
        // Generate != from operator==
        boost::equality_comparable<SystemLocation>,
        // Generate <=, >, >= from operator<
        boost::less_than_comparable<SystemLocation>
{
public:
    SystemLocation();
    SystemLocation(int system, int position);

    bool operator<(const SystemLocation &location) const;
    bool operator==(const SystemLocation &location) const;

    void setSystem(int system);
    int getSystem() const;

    void setPosition(int position);
    int getPosition() const;

private:
    int mySystem;
    int myPosition;
};

namespace std
{
    /// Enable the use of SystemLocation as a key for std::unordered_map, etc.
    template<>
    struct hash<SystemLocation>
    {
        size_t operator()(const SystemLocation &location) const;
    };
}

#endif
