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

#include <boost/range/iterator_range_core.hpp>
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
    typedef std::vector<System>::iterator SystemIterator;
    typedef std::vector<System>::const_iterator SystemConstIterator;
    typedef std::vector<Player>::iterator PlayerIterator;
    typedef std::vector<Player>::const_iterator PlayerConstIterator;
    typedef std::vector<Instrument>::iterator InstrumentIterator;
    typedef std::vector<Instrument>::const_iterator InstrumentConstIterator;

    bool operator==(const Score &other) const;

    /// Returns information about the score (e.g. title, author, etc.).
    const ScoreInfo &getScoreInfo() const;
    /// Sets information about the score (e.g. title, author, etc.).
    void setScoreInfo(const ScoreInfo &info);

    /// Returns the set of systems in the score.
    boost::iterator_range<SystemIterator> getSystems();
    /// Returns the set of systems in the score.
    boost::iterator_range<SystemConstIterator> getSystems() const;

    /// Adds a new system to the score.
    void insertSystem(const System &system);
    /// Removes the specified system from the score.
    void removeSystem(const System &system);

    /// Returns the set of players in the score.
    boost::iterator_range<PlayerIterator> getPlayers();
    /// Returns the set of players in the score.
    boost::iterator_range<PlayerConstIterator> getPlayers() const;

    /// Adds a new player to the score.
    void insertPlayer(const Player &player);
    /// Removes the specified player from the score.
    void removePlayer(const Player &player);

    /// Returns the set of instruments in the score.
    boost::iterator_range<InstrumentIterator> getInstruments();
    /// Returns the set of instruments in the score.
    boost::iterator_range<InstrumentConstIterator> getInstruments() const;

    /// Adds a new instrument to the score.
    void insertInstrument(const Instrument &instrument);
    /// Removes the specified instrument from the score.
    void removeInstrument(const Instrument &instrument);

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
