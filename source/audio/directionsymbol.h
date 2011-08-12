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
  
#ifndef DIRECTIONSYMBOL_H
#define DIRECTIONSYMBOL_H

#include <cstdint>

/// Convenient class for storing information about a single direction
/// (vs. the Direction class which represents multiple directions)
class DirectionSymbol
{
public:
    DirectionSymbol(uint8_t symbolType, uint8_t activeSymbol, uint8_t repeatNumber);

    uint8_t getActiveSymbol() const;
    uint8_t getSymbolType() const;
    uint8_t getRepeatNumber() const;

    bool shouldPerformDirection(uint8_t activeSymbol, uint8_t activeRepeat) const;

private:
    uint8_t symbolType;
    uint8_t activeSymbol;
    uint8_t repeatNumber;
};

#endif // DIRECTIONSYMBOL_H
