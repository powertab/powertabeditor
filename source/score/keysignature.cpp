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

#include "keysignature.h"

#include <ostream>
#include <stdexcept>
#include <string>
#include "generalmidi.h"

const uint8_t KeySignature::MAX_NUM_ACCIDENTALS = 7;

KeySignature::KeySignature()
    : myKeyType(Major),
      myNumAccidentals(0),
      myUsesSharps(true),
      myIsVisible(false),
      myIsCancellation(false)
{
}

KeySignature::KeySignature(KeyType type, uint8_t accidentals, bool usesSharps)
    : myKeyType(type),
      myNumAccidentals(accidentals),
      myUsesSharps(usesSharps),
      myIsVisible(false),
      myIsCancellation(false)
{
}

bool KeySignature::operator==(const KeySignature &other) const
{
    return myKeyType == other.myKeyType &&
           myNumAccidentals == other.myNumAccidentals &&
           myUsesSharps == other.myUsesSharps &&
           myIsVisible == other.myIsVisible &&
           myIsCancellation == other.myIsCancellation;
}

KeySignature::KeyType KeySignature::getKeyType() const
{
    return myKeyType;
}

void KeySignature::setKeyType(KeyType type)
{
    myKeyType = type;
}

uint8_t KeySignature::getNumAccidentals(bool includeCancel) const
{
    // Cancellations will always be C Major / A Minor, so if we are not
    // including the cancellation then there are no accidentals.
    if (isCancellation() && !includeCancel)
        return 0;

    return myNumAccidentals;
}

void KeySignature::setNumAccidentals(uint8_t accidentals)
{
    if (accidentals > MAX_NUM_ACCIDENTALS)
        throw std::out_of_range("Invalid number of accidentals");

    myNumAccidentals = accidentals;
}

bool KeySignature::usesSharps() const
{
    return myUsesSharps;
}

void KeySignature::setSharps(bool sharps)
{
    myUsesSharps = sharps;
}

bool KeySignature::isVisible() const
{
    return myIsVisible;
}

void KeySignature::setVisible(bool visible)
{
    myIsVisible = visible;
}

bool KeySignature::isCancellation() const
{
    return myIsCancellation;
}

void KeySignature::setCancellation(bool cancellation)
{
    myIsCancellation = cancellation;
}

std::ostream& operator<<(std::ostream &os, const KeySignature &key)
{
    const std::string type = (key.getKeyType() == KeySignature::Major) ?
        "Major" : "Minor";
    const std::string tonic = Midi::getKeyText(
            key.getKeyType() == KeySignature::Minor, key.usesSharps(),
            key.getNumAccidentals());
    const std::string separator = key.getNumAccidentals() == 0 ? "" : " -";

    os << tonic << " " << type << separator;

    for (int i = 0; i < key.getNumAccidentals(); ++i)
    {
        const int offset = key.usesSharps() ? 6 : 2;
        os << " " << Midi::getKeyText(false, key.usesSharps(), offset + i);
    }

    return os;
}

