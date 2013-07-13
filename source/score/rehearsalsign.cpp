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

#include "rehearsalsign.h"

RehearsalSign::RehearsalSign()
    : myLetters(""), myDescription("")
{
}

RehearsalSign::RehearsalSign(const std::string &letters,
                             const std::string &description)
    : myLetters(letters), myDescription(description)
{
}

bool RehearsalSign::operator==(const RehearsalSign &other) const
{
    return myLetters == other.myLetters &&
           myDescription == other.myDescription;
}

const std::string &RehearsalSign::getLetters() const
{
    return myLetters;
}

void RehearsalSign::setLetters(const std::string &letters)
{
    myLetters = letters;
}

const std::string &RehearsalSign::getDescription() const
{
    return myDescription;
}

void RehearsalSign::setDescription(const std::string &description)
{
    myDescription = description;
}

