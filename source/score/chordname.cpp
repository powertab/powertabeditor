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

#include "chordname.h"

ChordName::ChordName()
    : myTonicKey(C),
      myTonicVariation(NoVariation),
      myBassKey(C),
      myBassVariation(NoVariation),
      myFormula(Major),
      myHasBrackets(false),
      myIsNoChord(false)
{
}

bool ChordName::operator==(const ChordName &other) const
{
    return myTonicKey == other.myTonicKey &&
           myTonicVariation == other.myTonicVariation &&
           myBassKey == other.myBassKey &&
           myBassVariation == other.myBassVariation &&
           myFormula == other.myFormula &&
           myModifications == other.myModifications &&
           myHasBrackets == other.myHasBrackets &&
           myIsNoChord == other.myIsNoChord;
}

bool ChordName::isNoChord() const
{
    return myIsNoChord;
}

void ChordName::setNoChord(bool value)
{
    myIsNoChord = value;
}

bool ChordName::hasBrackets() const
{
    return myHasBrackets;
}

void ChordName::setBrackets(bool value)
{
    myHasBrackets = value;
}

ChordName::Formula ChordName::getFormula() const
{
    return myFormula;
}

void ChordName::setFormula(Formula formula)
{
    myFormula = formula;
}

bool ChordName::hasModification(
    ChordName::FormulaModification modification) const
{
    return myModifications.test(modification);
}

void ChordName::setModification(ChordName::FormulaModification modification,
                                bool set)
{
    myModifications.set(modification, set);
}

ChordName::Variation ChordName::getBassVariation() const
{
    return myBassVariation;
}

void ChordName::setBassVariation(Variation variation)
{
    myBassVariation = variation;
}

ChordName::Key ChordName::getBassKey() const
{
    return myBassKey;
}

void ChordName::setBassKey(Key key)
{
    myBassKey = key;
}

ChordName::Variation ChordName::getTonicVariation() const
{
    return myTonicVariation;
}

void ChordName::setTonicVariation(Variation variation)
{
    myTonicVariation = variation;
}

ChordName::Key ChordName::getTonicKey() const
{
    return myTonicKey;
}

void ChordName::setTonicKey(Key key)
{
    myTonicKey = key;
}
