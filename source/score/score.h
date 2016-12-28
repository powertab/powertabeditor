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
#include "fileversion.h"
#include "instrument.h"
#include "player.h"
#include "scoreinfo.h"
#include "system.h"
#include "viewfilter.h"
#include <vector>

class PlayerChange;

class Score
{
public:
    typedef std::vector<System>::iterator SystemIterator;
    typedef std::vector<System>::const_iterator SystemConstIterator;
    typedef std::vector<Player>::iterator PlayerIterator;
    typedef std::vector<Player>::const_iterator PlayerConstIterator;
    typedef std::vector<Instrument>::iterator InstrumentIterator;
    typedef std::vector<Instrument>::const_iterator InstrumentConstIterator;
    typedef std::vector<ViewFilter>::iterator ViewFilterIterator;
    typedef std::vector<ViewFilter>::const_iterator ViewFilterConstIterator;

    Score();
    Score(const Score &other) = delete;
    Score &operator=(const Score &other) = delete;
    bool operator==(const Score &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns information about the score (e.g. title, author, etc.).
    const ScoreInfo &getScoreInfo() const;
    /// Sets information about the score (e.g. title, author, etc.).
    void setScoreInfo(const ScoreInfo &info);

    /// Returns the set of systems in the score.
    boost::iterator_range<SystemIterator> getSystems();
    /// Returns the set of systems in the score.
    boost::iterator_range<SystemConstIterator> getSystems() const;

    /// Adds a new system to the score, optionally at a specific index.
    void insertSystem(const System &system, int index = -1);
    /// Removes the specified system from the score.
    void removeSystem(int index);

    /// Returns the set of players in the score.
    boost::iterator_range<PlayerIterator> getPlayers();
    /// Returns the set of players in the score.
    boost::iterator_range<PlayerConstIterator> getPlayers() const;

    /// Adds a new player to the score.
    void insertPlayer(const Player &player);
    void insertPlayer(const Player &player, int index);
    /// Removes the specified player from the score.
    void removePlayer(int index);

    /// Returns the set of instruments in the score.
    boost::iterator_range<InstrumentIterator> getInstruments();
    /// Returns the set of instruments in the score.
    boost::iterator_range<InstrumentConstIterator> getInstruments() const;

    /// Adds a new instrument to the score.
    void insertInstrument(const Instrument &instrument);
    void insertInstrument(const Instrument &instrument, int index);
    /// Removes the specified instrument from the score.
    void removeInstrument(int index);

    /// Returns the set of view filters in the score.
    boost::iterator_range<ViewFilterIterator> getViewFilters();
    /// Returns the set of view filters in the score.
    boost::iterator_range<ViewFilterConstIterator> getViewFilters() const;

    /// Adds a new filter to the score.
    void insertViewFilter(const ViewFilter &filter);
    /// Removes the specified filter from the score.
    void removeViewFilter(int index);

    /// Returns the spacing between tabulature lines for the score.
    int getLineSpacing() const;
    /// Sets the spacing between tabulature lines for the score.
    void setLineSpacing(int value);

    static const int MIN_LINE_SPACING;
    static const int MAX_LINE_SPACING;

private:
    // TODO - add font settings, chord diagrams, etc.
    ScoreInfo myScoreInfo;
    std::vector<System> mySystems;
    std::vector<Player> myPlayers;
    std::vector<Instrument> myInstruments;
    int myLineSpacing; ///< Spacing between tab lines (in pixels).
    std::vector<ViewFilter> myViewFilters;
};

template <class Archive>
void Score::serialize(Archive &ar, const FileVersion version)
{
    ar("score_info", myScoreInfo);
    ar("systems", mySystems);
    ar("players", myPlayers);
    ar("instruments", myInstruments);
    ar("line_spacing", myLineSpacing);

    if (version >= FileVersion::VIEW_FILTERS)
        ar("view_filters", myViewFilters);
}

namespace ScoreUtils {
/// Get the current player change for the given position.
const PlayerChange *getCurrentPlayers(const Score &score, int systemIndex,
                                      int positionIndex);

/// Readjust the letters for the rehearsal signs in the score
/// (i.e. assigning rehearsal signs the letters "A", "B", and so on).
void adjustRehearsalSigns(Score &score);

/// Add the standard view filters (guitar and bass) to the score.
void addStandardFilters(Score &score);
}

#endif
