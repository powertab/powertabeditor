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

#include <memory>

/// Convenience class to represent a location in a system.
class SystemLocation
{
public:
    SystemLocation() = default;
    SystemLocation(int system, int position);

    auto operator<=>(const SystemLocation &location) const = default;

    void setSystem(int system);
    int getSystem() const;

    void setPosition(int position);
    int getPosition() const;

private:
    int mySystem = 0;
    int myPosition = 0;
};

/// Enable the use of SystemLocation as a key for std::unordered_map, etc.
template<>
struct std::hash<SystemLocation>
{
    size_t operator()(const SystemLocation &location) const;
};

#endif
