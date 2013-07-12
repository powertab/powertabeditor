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

#include "direction.h"

#include <algorithm>

namespace Score {

Direction::Direction()
    : myPosition(0)
{
}

Direction::Direction(int position)
    : myPosition(position)
{
}

bool Direction::operator==(const Direction &other) const
{
    return myPosition == other.myPosition &&
           mySymbols == other.mySymbols;
}

int Direction::getPosition() const
{
    return myPosition;
}

void Direction::setPosition(int position)
{
    myPosition = position;
}

boost::iterator_range<Direction::SymbolIterator> Direction::getSymbols()
{
    return boost::make_iterator_range(mySymbols);
}

boost::iterator_range<Direction::SymbolConstIterator> Direction::getSymbols() const
{
    return boost::make_iterator_range(mySymbols);
}

void Direction::insertSymbol(const DirectionSymbol &symbol)
{
    mySymbols.push_back(symbol);
}

void Direction::removeSymbol(const DirectionSymbol &symbol)
{
    mySymbols.erase(std::remove(mySymbols.begin(), mySymbols.end(), symbol),
                    mySymbols.end());
}

DirectionSymbol::DirectionSymbol()
    : mySymbolType(Coda),
      myActiveSymbolType(ActiveNone),
      myRepeatNumber(0)
{
}

DirectionSymbol::DirectionSymbol(SymbolType type, ActiveSymbolType activeType,
                                 int repeatNumber)
    : mySymbolType(type),
      myActiveSymbolType(activeType),
      myRepeatNumber(repeatNumber)
{
}

bool DirectionSymbol::operator==(const DirectionSymbol &other) const
{
    return mySymbolType == other.mySymbolType &&
           myActiveSymbolType == other.myActiveSymbolType &&
           myRepeatNumber == other.myRepeatNumber;
}

DirectionSymbol::SymbolType DirectionSymbol::getSymbolType() const
{
    return mySymbolType;
}

DirectionSymbol::ActiveSymbolType DirectionSymbol::getActiveSymbolType() const
{
    return myActiveSymbolType;
}

int DirectionSymbol::getRepeatNumber() const
{
    return myRepeatNumber;
}

}
