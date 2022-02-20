/*
 * Copyright (C) 2022 Cameron White
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

#include "chorddiagram.h"

#include "tuning.h"
#include <boost/functional/hash.hpp>
#include <ostream>

ChordDiagram::ChordDiagram()
{
}

bool ChordDiagram::operator==(const ChordDiagram &other) const
{
    return myChordName == other.myChordName && myTopFret == other.myTopFret &&
           myFretNumbers == other.myFretNumbers;
}

void
ChordDiagram::setStringCount(int count)
{
    if (!Tuning::isValidStringCount(count))
        throw std::out_of_range("Invalid string count");

    if (count > getStringCount())
    {
        for (int i = getStringCount(); i < count; ++i)
            myFretNumbers.push_back(-1);
    }
    else
        myFretNumbers.resize(count);
}

std::ostream &
operator<<(std::ostream &os, const ChordDiagram &diagram)
{
    os << diagram.getChordName() << ": ";

    for (int i = diagram.getStringCount() - 1; i >= 0; --i)
    {
        int fret = diagram.getFretNumbers()[i];
        if (fret < 0)
            os << "x";
        else
            os << fret;

        if (i > 0)
            os << " ";
    }

    if (diagram.getTopFret() > 0)
        os << " (" << (diagram.getTopFret() + 1) << ")";

    return os;
}

size_t
std::hash<ChordDiagram>::operator()(const ChordDiagram &diagram) const
{
    size_t seed = std::hash<ChordName>()(diagram.getChordName());
    boost::hash_combine(seed, diagram.getTopFret());
    boost::hash_range(seed, diagram.getFretNumbers().begin(),
                      diagram.getFretNumbers().end());
    return seed;
}
