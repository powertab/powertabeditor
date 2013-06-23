/*
  * Copyright (C) 2011 Cameron White
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
  
#ifndef SCOREARRANGER_H
#define SCOREARRANGER_H

#include <boost/shared_ptr.hpp>
#include <vector>

class Barline;
class Position;
class Score;
class AlternateEnding;
class TempoMarker;

/// Stores data for a single position, plus any symbols at that position.
struct PositionData
{
    PositionData(Position *position);
    PositionData(const PositionData &other);
    ~PositionData();
    PositionData &operator=(const PositionData &other);

    // TODO - when we can use C++11, this can just be a unique_ptr.
    Position *position;
    boost::shared_ptr<TempoMarker> tempoMarker;
};

/// Stores data for a single bar - barline, an (optional) alternate ending,
/// and a list of positions (one for each staff)
struct BarData
{
    boost::shared_ptr<Barline> barline;
    boost::shared_ptr<AlternateEnding> altEnding;
    boost::shared_ptr<TempoMarker> tempoMarker;
    std::vector<std::vector<PositionData> > positionLists;
};

void arrangeScore(Score* score, const std::vector<BarData>& bars,
                  bool fixBarlineTypes);

#endif // SCOREARRANGER_H
