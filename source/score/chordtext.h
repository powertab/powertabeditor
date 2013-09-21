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

#ifndef SCORE_CHORDTEXT_H
#define SCORE_CHORDTEXT_H

#include <boost/serialization/access.hpp>
#include "chordname.h"

class ChordText
{
public:
    ChordText();
    ChordText(int position, const ChordName &name);

    bool operator==(const ChordText &other) const;

    int getPosition() const;
    void setPosition(int position);

    const ChordName &getChordName() const;
    void setChordName(const ChordName &name);

private:
    int myPosition;
    ChordName myChordName;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myPosition & myChordName;
    }
};

#endif
