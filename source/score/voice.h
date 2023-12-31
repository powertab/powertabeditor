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

#include "fileversion.h"
#include "irregulargrouping.h"
#include "position.h"
#include <span>
#include <vector>

class Voice
{
public:
    Voice();

    bool operator==(const Voice &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the set of positions in the voice.
    std::span<Position> getPositions() { return myPositions; }
    /// Returns the set of positions in the voice.
    std::span<const Position> getPositions() const { return myPositions; }

    /// @{
    /// Adds a new position to the voice.
    void insertPosition(const Position &position);
    void insertPosition(Position &&position);
    /// @}
    /// Removes any positions that satisfy the given predicate.
    template <typename Predicate>
    void removePositions(Predicate p);
    /// Removes the specified position from the voice.
    void removePosition(const Position &position);

    /// Increase the capacity for the list of positions.
    void setPositionsCapacity(size_t capacity) { myPositions.reserve(capacity); }

    /// Returns the set of irregular groupings in the voice.
    std::span<IrregularGrouping> getIrregularGroupings() { return myIrregularGroupings; }
    /// Returns the set of irregular groupings in the voice.
    std::span<const IrregularGrouping> getIrregularGroupings() const { return myIrregularGroupings; }

    /// Adds a new irregular grouping to the voice.
    void insertIrregularGrouping(const IrregularGrouping &group);
    /// Removes the specified irregular grouping from the voice.
    void removeIrregularGrouping(const IrregularGrouping &group);

private:
    std::vector<Position> myPositions;
    std::vector<IrregularGrouping> myIrregularGroupings;
};

template <class Archive>
void Voice::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("positions", myPositions);
    ar("irregular_groupings", myIrregularGroupings);
}

template <typename Predicate>
void Voice::removePositions(Predicate p)
{
    myPositions.erase(std::remove_if(myPositions.begin(), myPositions.end(), p),
                      myPositions.end());
}

#endif
