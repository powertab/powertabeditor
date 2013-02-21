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
  
#include "directionsymbol.h"

#include <powertabdocument/direction.h>

DirectionSymbol::DirectionSymbol(uint8_t symbolType, uint8_t activeSymbol, uint8_t repeatNumber) :
    symbolType(symbolType),
    activeSymbol(activeSymbol),
    repeatNumber(repeatNumber)
{
}

uint8_t DirectionSymbol::getActiveSymbol() const
{
    return activeSymbol;
}

uint8_t DirectionSymbol::getRepeatNumber() const
{
    return repeatNumber;
}

uint8_t DirectionSymbol::getSymbolType() const
{
    return symbolType;
}

/// Determines whether the direction should be performed, based on the
/// active symbol and repeat number.
/// If the direction's activation symbol is None, it will always be able to
/// activate regardless of the currently active symbol.
bool DirectionSymbol::shouldPerformDirection(uint8_t activeSymbol, uint8_t activeRepeat) const
{
    return ((this->activeSymbol == Direction::activeNone ||
             this->activeSymbol == activeSymbol) &&
            symbolType >= Direction::fine &&
            (repeatNumber == 0 || repeatNumber == activeRepeat));
}
