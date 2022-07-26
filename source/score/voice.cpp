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

#include "voice.h"

#include "utils.h"

Voice::Voice()
{
}

bool Voice::operator==(const Voice &other) const
{
    return myPositions == other.myPositions &&
           myIrregularGroupings == other.myIrregularGroupings;
}

boost::iterator_range<Voice::PositionIterator> Voice::getPositions()
{
    return boost::make_iterator_range(myPositions);
}

boost::iterator_range<Voice::PositionConstIterator> Voice::getPositions() const
{
    return boost::make_iterator_range(myPositions);
}

void Voice::insertPosition(const Position &position)
{
    ScoreUtils::insertObject(myPositions, position);
}

void Voice::insertPosition(Position &&position)
{
    ScoreUtils::insertObject(myPositions, std::move(position));
}

void Voice::removePosition(const Position &position)
{
    ScoreUtils::removeObject(myPositions, position);
}

boost::iterator_range<Voice::IrregularGroupingIterator>
Voice:: getIrregularGroupings()
{
    return boost::make_iterator_range(myIrregularGroupings);
}

boost::iterator_range<Voice::IrregularGroupingConstIterator>
Voice:: getIrregularGroupings() const
{
    return boost::make_iterator_range(myIrregularGroupings);
}

void Voice::insertIrregularGrouping(const IrregularGrouping &group)
{
    ScoreUtils::insertObject(myIrregularGroupings, group);
}

void Voice::removeIrregularGrouping(const IrregularGrouping &group)
{
    ScoreUtils::removeObject(myIrregularGroupings, group);
}
