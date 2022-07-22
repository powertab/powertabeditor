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

#include "score.h"

#include <boost/range/adaptor/reversed.hpp>
#include <stdexcept>
#include <unordered_set>
#include <util/tostring.h>

const int Score::MIN_LINE_SPACING = 6;
const int Score::MAX_LINE_SPACING = 14;

Score::Score()
    : myLineSpacing(9)
{
}

bool Score::operator==(const Score &other) const
{
    return myScoreInfo == other.myScoreInfo && mySystems == other.mySystems &&
           myPlayers == other.myPlayers &&
           myInstruments == other.myInstruments &&
           myChordDiagrams == other.myChordDiagrams &&
           myLineSpacing == other.myLineSpacing &&
           myViewFilters == other.myViewFilters;
}

const ScoreInfo &Score::getScoreInfo() const
{
    return myScoreInfo;
}

void Score::setScoreInfo(const ScoreInfo &info)
{
    myScoreInfo = info;
}

boost::iterator_range<Score::SystemIterator> Score::getSystems()
{
    return boost::make_iterator_range(mySystems);
}

boost::iterator_range<Score::SystemConstIterator> Score::getSystems() const
{
    return boost::make_iterator_range(mySystems);
}

void Score::insertSystem(const System &system, int index)
{
    if (index < 0)
        mySystems.push_back(system);
    else
        mySystems.insert(mySystems.begin() + index, system);
}

void Score::removeSystem(int index)
{
    mySystems.erase(mySystems.begin() + index);
}

boost::iterator_range<Score::PlayerIterator> Score::getPlayers()
{
    return boost::make_iterator_range(myPlayers);
}

boost::iterator_range<Score::PlayerConstIterator> Score::getPlayers() const
{
    return boost::make_iterator_range(myPlayers);
}

void Score::insertPlayer(const Player &player)
{
    myPlayers.push_back(player);
}

void Score::insertPlayer(const Player &player, int index)
{
    myPlayers.insert(myPlayers.begin() + index, player);
}

void Score::removePlayer(int index)
{
    myPlayers.erase(myPlayers.begin() + index);
}

boost::iterator_range<Score::InstrumentIterator> Score::getInstruments()
{
    return boost::make_iterator_range(myInstruments);
}

boost::iterator_range<Score::InstrumentConstIterator> Score::getInstruments() const
{
    return boost::make_iterator_range(myInstruments);
}

void Score::insertInstrument(const Instrument &instrument)
{
    myInstruments.push_back(instrument);
}

void Score::insertInstrument(const Instrument &instrument, int index)
{
    myInstruments.insert(myInstruments.begin() + index, instrument);
}

void Score::removeInstrument(int index)
{
    myInstruments.erase(myInstruments.begin() + index);
}

boost::iterator_range<Score::ChordDiagramIterator>
Score::getChordDiagrams()
{
    return boost::make_iterator_range(myChordDiagrams);
}

boost::iterator_range<Score::ChordDiagramConstIterator>
Score::getChordDiagrams() const
{
    return boost::make_iterator_range(myChordDiagrams);
}

void
Score::insertChordDiagram(const ChordDiagram &filter)
{
    myChordDiagrams.push_back(filter);
}

void Score::insertChordDiagram(const ChordDiagram &diagram, int index)
{
    myChordDiagrams.insert(myChordDiagrams.begin() + index, diagram);
}

void
Score::removeChordDiagram(int index)
{
    myChordDiagrams.erase(myChordDiagrams.begin() + index);
}

boost::iterator_range<Score::ViewFilterIterator> Score::getViewFilters()
{
    return boost::make_iterator_range(myViewFilters);
}

boost::iterator_range<Score::ViewFilterConstIterator> Score::getViewFilters() const
{
    return boost::make_iterator_range(myViewFilters);
}

void Score::insertViewFilter(const ViewFilter &filter)
{
    myViewFilters.push_back(filter);
}

void Score::removeViewFilter(int index)
{
    myViewFilters.erase(myViewFilters.begin() + index);
}

int Score::getLineSpacing() const
{
    return myLineSpacing;
}

void Score::setLineSpacing(int value)
{
    if (value < MIN_LINE_SPACING || value > MAX_LINE_SPACING)
        throw std::out_of_range("Invalid line spacing");

    myLineSpacing = value;
}

template <typename T, typename SymbolRangeAccessor>
static const T *
findCurrentSymbol(const Score &score, int system_idx, int position_idx,
                  SymbolRangeAccessor get_symbols)
{
    // First check the current system.
    {
        const System &system = score.getSystems()[system_idx];
        for (const T &obj : boost::adaptors::reverse(get_symbols(system)))
        {
            if (obj.getPosition() <= position_idx)
                return &obj;
        }
    }

    // Otherwise, search backwards to find the last symbol from a previous
    // system.
    for (int i = system_idx - 1; i >= 0; --i)
    {
        const System &system = score.getSystems()[i];
        auto &&symbols = get_symbols(system);
        if (!symbols.empty())
            return &symbols.back();
    }

    return nullptr;
}

const PlayerChange *
ScoreUtils::getCurrentPlayers(const Score &score, int system_idx,
                              int position_idx)
{
    return findCurrentSymbol<PlayerChange>(
        score, system_idx, position_idx,
        [](const System &system) { return system.getPlayerChanges(); });
}

const ChordText *
ScoreUtils::getCurrentChordText(const Score &score, int system_idx,
                                int position_idx)
{
    return findCurrentSymbol<ChordText>(
        score, system_idx, position_idx,
        [](const System &system) { return system.getChords(); });
}

void ScoreUtils::adjustRehearsalSigns(Score &score)
{
    std::string letters;
    char letter = 'Z';

    for (System &system : score.getSystems())
    {
        for (Barline &barline : system.getBarlines())
        {
            if (barline.hasRehearsalSign())
            {
                RehearsalSign &sign = barline.getRehearsalSign();

                // Cycle through the letters A-Z, and then to AA, AB, etc.
                if (letter == 'Z')
                {
                    letter = 'A';
					if (!letters.empty())
						letters.back() = letter;
                    letters.push_back(letter);
                }
                else
                {
                    ++letter;
					letters.back() = letter;
                }

                sign.setLetters(letters);
            }
        }
    }
}

void ScoreUtils::addStandardFilters(Score &score)
{
    ViewFilter filter_all;
    filter_all.setDescription("All Instruments");
    score.insertViewFilter(filter_all);

    ViewFilter filter_guitars;
    filter_guitars.setDescription("Guitars");
    filter_guitars.addRule(FilterRule(FilterRule::Subject::NumStrings,
                                      FilterRule::Operation::GreaterThanEqual,
                                      6));
    score.insertViewFilter(filter_guitars);

    ViewFilter filter_basses;
    filter_basses.setDescription("Basses");
    filter_basses.addRule(FilterRule(FilterRule::Subject::NumStrings,
                                     FilterRule::Operation::LessThanEqual,
                                     5));
    score.insertViewFilter(filter_basses);
}

std::vector<ChordName>
ScoreUtils::findAllChordNames(const Score &score)
{
    std::unordered_set<ChordName> unique_names;

    for (const ChordDiagram &diagram : score.getChordDiagrams())
        unique_names.insert(diagram.getChordName());

    for (const System &system : score.getSystems())
    {
        for (const ChordText &chord : system.getChords())
            unique_names.insert(chord.getChordName());
    }

    std::vector<ChordName> names(unique_names.begin(), unique_names.end());
    // Sort the names alphabetically. The performance of this could be improved
    // by implementing operator< for ChordName's.
    std::sort(names.begin(), names.end(),
              [](auto c1, auto c2)
              { return Util::toString(c1) < Util::toString(c2); });
    return names;
}

static std::string
createUniqueName(size_t count, const std::string &prefix,
                 const std::unordered_set<std::string> &used_names)
{
    std::string name;
    do
    {
        ++count;
        name = prefix + " " + std::to_string(count);
    } while (used_names.find(name) != used_names.end());

    return name;
}

std::string
ScoreUtils::createUniquePlayerName(const Score &score,
                                   const std::string &prefix)
{
    std::unordered_set<std::string> used_names;
    for (const Player &player : score.getPlayers())
        used_names.insert(player.getDescription());

    return createUniqueName(score.getPlayers().size(), prefix, used_names);
}

std::string
ScoreUtils::createUniqueInstrumentName(const Score &score,
                                       const std::string &prefix)
{
    std::unordered_set<std::string> used_names;
    for (const Instrument &inst : score.getInstruments())
        used_names.insert(inst.getDescription());

    return createUniqueName(score.getInstruments().size(), prefix, used_names);
}
