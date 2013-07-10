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

#ifndef SCORE_POSITION_H
#define SCORE_POSITION_H

#include <boost/serialization/access.hpp>

namespace Score {

class Position
{
public:
    Position();
    Position(int position);

    bool operator==(const Position &other) const;

    /// Returns the position within the staff where the position is anchored.
    int getPosition() const;
    /// Sets the position within the staff where the position is anchored.
    void setPosition(int position);

private:
    int myPosition;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myPosition;
    }
};

}

#endif
