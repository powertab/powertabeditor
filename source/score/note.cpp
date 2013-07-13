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

#include "note.h"

#include <stdexcept>

namespace Score {

Note::Note()
    : myString(0),
      myFretNumber(0),
      myTrilledFret(-1),
      myTappedHarmonicFret(-1)
{
}

Note::Note(int string, int fretNumber)
    : myString(string),
      myFretNumber(fretNumber),
      myTrilledFret(-1),
      myTappedHarmonicFret(-1)
{
}

bool Note::operator==(const Note &other) const
{
    return myString == other.myString &&
           myFretNumber == other.myFretNumber &&
           mySimpleProperties == other.mySimpleProperties &&
           myTrilledFret == other.myTrilledFret &&
           myTappedHarmonicFret == other.myTappedHarmonicFret;
}

int Note::getString() const
{
    return myString;
}

void Note::setString(int string)
{
    myString = string;
}

int Note::getFretNumber() const
{
    return myFretNumber;
}

void Note::setFretNumber(int fret)
{
    myFretNumber = fret;
}

bool Note::hasProperty(SimpleProperty property) const
{
    return mySimpleProperties.test(property);
}

void Note::setProperty(SimpleProperty property, bool set)
{
    mySimpleProperties.set(property, set);
}

bool Note::hasTrill() const
{
    return myTrilledFret != -1;
}

int Note::getTrilledFret() const
{
    if (!hasTrill())
        throw std::logic_error("Note does not have a trill");

    return myTrilledFret;
}

void Note::setTrilledFret(int fret)
{
    if (fret < 0)
        throw std::out_of_range("Invalid fret number");

    myTrilledFret = fret;
}

void Note::clearTrill()
{
    myTrilledFret = -1;
}

bool Note::hasTappedHarmonic() const
{
    return myTappedHarmonicFret != -1;
}

int Note::getTappedHarmonicFret() const
{
    if (!hasTappedHarmonic())
        throw std::logic_error("Note does not have a tapped harmonic");

    return myTappedHarmonicFret;
}

void Note::setTappedHarmonicFret(int fret)
{
    if (fret < 0)
        throw std::out_of_range("Invalid fret number");

    myTappedHarmonicFret = fret;
}

void Note::clearTappedHarmonic()
{
    myTappedHarmonicFret = -1;
}

}
