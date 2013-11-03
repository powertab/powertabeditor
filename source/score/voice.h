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

#ifndef SCORE_VOICE_H
#define SCORE_VOICE_H

#include <boost/range/iterator_range_core.hpp>
#include "fileversion.h"
#include "position.h"
#include <vector>

class Voice
{
public:
    Voice();

    typedef std::vector<Position>::iterator PositionIterator;
    typedef std::vector<Position>::const_iterator PositionConstIterator;

    bool operator==(const Voice &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the set of positions in the voice.
    boost::iterator_range<PositionIterator> getPositions();
    /// Returns the set of positions in the voice.
    boost::iterator_range<PositionConstIterator> getPositions() const;

    /// Adds a new position to the voice.
    void insertPosition(const Position &position);
    /// Removes the specified position from the voice.
    void removePosition(const Position &position);

private:
    std::vector<Position> myPositions;
};

template <class Archive>
void Voice::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("positions", myPositions);
}

#endif
