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

#include <array>
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <iostream>

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

std::ostream &operator<<(std::ostream &os, const ChordName &chord)
{
    static std::string theKeys[] = { "C", "D", "E", "F", "G", "A", "B" };
    static std::string theVariations[] = { "bb", "b", "", "#", "x" };
    static std::string theSuffixes[] = { "",   "m",  "+",      "°",    "5",
                                         "6",  "m6", "7",      "maj7", "m7",
                                         "+7", "°7", "m/maj7", "m7b5" };
    static std::array<ChordName::FormulaModification, 13> theModifications = {
        { ChordName::Suspended2nd, ChordName::Suspended4th,
          ChordName::Added2nd,     ChordName::Added4th,
          ChordName::Added6th,     ChordName::Added9th,
          ChordName::Added11th,    ChordName::Flatted13th,
          ChordName::Raised11th,   ChordName::Flatted9th,
          ChordName::Raised9th,    ChordName::Flatted5th,
          ChordName::Raised5th }
    };
    static std::string theModificationText[] = {
        "sus2", "sus4", "add2", "add4", "add6", "add9", "add11",
        "b13",  "+11",  "b9",   "+9",   "b5",   "+5"
    };

    if (chord.isNoChord())
    {
        os << "N.C.";

        // Unless the chord has brackets, we're done.
        if (!chord.hasBrackets())
            return os;
    }

    if (chord.hasBrackets())
        os << "(";

    os << theKeys[chord.getTonicKey()];
    os << theVariations[chord.getTonicVariation() - ChordName::DoubleFlat];

    // Display the chord formula.
    {
        std::string formula = theSuffixes[chord.getFormula()];

        // Handle chord extensions.
        if (chord.hasModification(ChordName::Extended9th))
            boost::algorithm::replace_first(formula, "7", "9");
        else if (chord.hasModification(ChordName::Extended11th))
            boost::algorithm::replace_first(formula, "7", "11");
        else if (chord.hasModification(ChordName::Extended13th))
            boost::algorithm::replace_first(formula, "7", "13");

        os << formula;

        // Display modifications such as 'add9'.
        for (size_t i = 0; i < theModifications.size(); ++i)
        {
            if (chord.hasModification(theModifications[i]))
                os << theModificationText[i];
        }
    }

    // If the tonic key and bass note are different, display the bass note.
    if (chord.getTonicKey() != chord.getBassKey() ||
        chord.getTonicVariation() != chord.getBassVariation())
    {
        os << "/";
        os << theKeys[chord.getBassKey()];
        os << theVariations[chord.getBassVariation() - ChordName::DoubleFlat];
    }

    if (chord.hasBrackets())
        os << ")";

    return os;
}
