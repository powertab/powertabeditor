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

#ifndef SCORE_SCORE_H
#define SCORE_SCORE_H

#include <boost/serialization/access.hpp>
#include <vector>
#include "instrument.h"
#include "player.h"
#include "scoreinfo.h"
#include "system.h"

namespace Score {

class Score
{
public:
    bool operator==(const Score &other) const;

    const ScoreInfo &getScoreInfo() const;
    void setScoreInfo(const ScoreInfo &info);

private:
    // TODO - add line spacing, font settings, chord diagrams, etc.
    ScoreInfo myScoreInfo;
    std::vector<System> mySystems;
    std::vector<Player> myPlayers;
    std::vector<Instrument> myInstruments;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myScoreInfo & mySystems & myPlayers & myInstruments;
    }
};

}

#endif
