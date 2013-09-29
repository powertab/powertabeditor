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

#ifndef SCORE_CHORDNAME_H
#define SCORE_CHORDNAME_H

#include <bitset>
#include "fileversion.h"
#include <iosfwd>

class ChordName
{
public:
    enum Key
    {
        C,
        D,
        E,
        F,
        G,
        A,
        B,
        NumKeys
    };

    enum Variation
    {
        DoubleFlat = -2,
        Flat = -1,
        NoVariation = 0,
        Sharp = 1,
        DoubleSharp = 2
    };

    enum Formula
    {
        Major,
        Minor,
        Augmented,
        Diminished,
        PowerChord,
        Major6th,
        Minor6h,
        Dominant7th,
        Major7th,
        Minor7th,
        Augmented7th,
        Diminished7th,
        MinorMajor7th,
        Minor7thFlatted5th
    };

    enum FormulaModification
    {
        Extended9th,
        Extended11th,
        Extended13th,
        Added2nd,
        Added4th,
        Added6th,
        Added9th,
        Added11th,
        Flatted5th,
        Raised5th,
        Flatted9th,
        Raised9th,
        Raised11th,
        Flatted13th,
        Suspended2nd,
        Suspended4th,
        NumModifications
    };

    ChordName();

    bool operator==(const ChordName &other) const;

	template <class Archive>
	void serialize(Archive &ar, const FileVersion version);

    Key getTonicKey() const;
    void setTonicKey(Key key);

    Variation getTonicVariation() const;
    void setTonicVariation(Variation variation);

    Key getBassKey() const;
    void setBassKey(Key key);

    Variation getBassVariation() const;
    void setBassVariation(Variation variation);

    Formula getFormula() const;
    void setFormula(Formula formula);

    bool hasModification(FormulaModification modification) const;
    void setModification(FormulaModification modification, bool set = true);

    bool hasBrackets() const;
    void setBrackets(bool value);

    bool isNoChord() const;
    void setNoChord(bool value);

private:
    Key myTonicKey;
    Variation myTonicVariation;
    Key myBassKey;
    Variation myBassVariation;
    Formula myFormula;
    std::bitset<NumModifications> myModifications;
    bool myHasBrackets;
    bool myIsNoChord;
};

template <class Archive>
void ChordName::serialize(Archive &ar, const FileVersion version)
{
	ar("tonic_key", myTonicKey);
	ar("tonic_variation", myTonicVariation);
	ar("bass_key", myBassKey);
	ar("bass_variation", myBassVariation);
	ar("formula", myFormula);
	ar("modifications", myModifications);
	ar("brackets", myHasBrackets);
	ar("no_chord", myIsNoChord);
}

std::ostream &operator<<(std::ostream &os, const ChordName &chord);

#endif
