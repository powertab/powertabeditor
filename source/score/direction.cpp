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

#include <util/enumtostring.h>

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

void Direction::removeSymbol(int index)
{
    mySymbols.erase(mySymbols.begin() + index);
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

using SymbolType = DirectionSymbol::SymbolType;
using ActiveSymbolType = DirectionSymbol::ActiveSymbolType;

UTIL_DEFINE_ENUMTOSTRING(SymbolType, {
    { SymbolType::Coda, "Coda" },
    { SymbolType::DoubleCoda, "DoubleCoda" },
    { SymbolType::Segno, "Segno" },
    { SymbolType::SegnoSegno, "SegnoSegno" },
    { SymbolType::Fine, "Fine" },
    { SymbolType::DaCapo, "DaCapo" },
    { SymbolType::DalSegno, "DalSegno" },
    { SymbolType::DalSegnoSegno, "DalSegnoSegno" },
    { SymbolType::ToCoda, "ToCoda" },
    { SymbolType::ToDoubleCoda, "ToDoubleCoda" },
    { SymbolType::DaCapoAlCoda, "DaCapoAlCoda" },
    { SymbolType::DaCapoAlDoubleCoda, "DaCapoAlDoubleCoda" },
    { SymbolType::DalSegnoAlCoda, "DalSegnoAlCoda" },
    { SymbolType::DalSegnoAlDoubleCoda, "DalSegnoAlDoubleCoda" },
    { SymbolType::DalSegnoSegnoAlCoda, "DalSegnoSegnoAlCoda" },
    { SymbolType::DalSegnoSegnoAlDoubleCoda, "DalSegnoSegnoAlDoubleCoda" },
    { SymbolType::DaCapoAlFine, "DaCapoAlFine" },
    { SymbolType::DalSegnoAlFine, "DalSegnoAlFine" },
    { SymbolType::DalSegnoSegnoAlFine, "DalSegnoSegnoAlFine" },
})

UTIL_DEFINE_ENUMTOSTRING(ActiveSymbolType, {
    { ActiveSymbolType::ActiveNone, "None" },
    { ActiveSymbolType::ActiveDaCapo, "DaCapo" },
    { ActiveSymbolType::ActiveDalSegno, "DalSegno" },
    { ActiveSymbolType::ActiveDalSegnoSegno, "DalSegnoSegno" },
})
