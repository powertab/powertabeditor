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

#include "alternateending.h"

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>

const int AlternateEnding::MIN_NUMBER = 1;
const int AlternateEnding::MAX_NUMBER = 8;

AlternateEnding::AlternateEnding()
    : myPosition(0)
{
}

AlternateEnding::AlternateEnding(int position)
    : myPosition(position)
{
}

bool AlternateEnding::operator==(const AlternateEnding &other) const
{
    return myPosition == other.myPosition &&
           myNumbers == other.myNumbers &&
           mySpecialEndings == other.mySpecialEndings;
}

int AlternateEnding::getPosition() const
{
    return myPosition;
}

void AlternateEnding::setPosition(int position)
{
    myPosition = position;
}

void AlternateEnding::addNumber(int number)
{
    if (number < MIN_NUMBER || number > MAX_NUMBER)
        throw std::out_of_range("Invalid alternate ending number");
    else if (std::find(myNumbers.begin(), myNumbers.end(), number) != myNumbers.end())
        throw std::logic_error("Cannot add duplicate alternate ending number");

    myNumbers.push_back(number);
    std::sort(myNumbers.begin(), myNumbers.end());
}

void AlternateEnding::removeNumber(int number)
{
    myNumbers.erase(std::remove(myNumbers.begin(), myNumbers.end(), number),
                    myNumbers.end());
}

const std::vector<int> &AlternateEnding::getNumbers() const
{
    return myNumbers;
}

bool AlternateEnding::hasDaCapo() const
{
    return mySpecialEndings[DaCapo];
}

void AlternateEnding::setDaCapo(bool set)
{
    mySpecialEndings[DaCapo] = set;
}

bool AlternateEnding::hasDalSegno() const
{
    return mySpecialEndings[DalSegno];
}

void AlternateEnding::setDalSegno(bool set)
{
    mySpecialEndings[DalSegno] = set;
}

bool AlternateEnding::hasDalSegnoSegno() const
{
    return mySpecialEndings[DalSegnoSegno];
}

void AlternateEnding::setDalSegnoSegno(bool set)
{
    mySpecialEndings[DalSegnoSegno] = set;
}

std::ostream &operator<<(std::ostream &os, const AlternateEnding &ending)
{
    const std::vector<int> &numbers = ending.getNumbers();
    std::vector<std::string> text;

    // Construct the numbers.
    for (int n : numbers)
        text.push_back(std::to_string(n) + ".");

    // Construct the special symbols
    if (ending.hasDaCapo())
        text.push_back("D.C");
    if (ending.hasDalSegno())
        text.push_back("D.S.");
    if (ending.hasDalSegnoSegno())
        text.push_back("D.S.S.");

    for (size_t i = 0; i < text.size(); ++i)
    {
        if (i != 0)
            os << ", ";
        os << text[i];
    }

    return os;
}
