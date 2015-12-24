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

const PlayerChange *ScoreUtils::getCurrentPlayers(const Score &score,
                                                  int systemIndex,
                                                  int positionIndex)
{
    const PlayerChange *lastChange = nullptr;

    int i = 0;
    for (const System &system : score.getSystems())
    {
        if (i > systemIndex)
            break;

        for (const PlayerChange &change : system.getPlayerChanges())
        {
            if (i < systemIndex ||
               (i == systemIndex && change.getPosition() <= positionIndex))
            {
                lastChange = &change;
            }
        }

        ++i;
    }

    return lastChange;
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
    filter_guitars.addRule(
        FilterRule(FilterRule::NUM_STRINGS, FilterRule::GREATER_THAN_EQUAL, 6));
    score.insertViewFilter(filter_guitars);

    ViewFilter filter_basses;
    filter_basses.setDescription("Basses");
    filter_basses.addRule(
        FilterRule(FilterRule::NUM_STRINGS, FilterRule::LESS_THAN_EQUAL, 5));
    score.insertViewFilter(filter_basses);
}
