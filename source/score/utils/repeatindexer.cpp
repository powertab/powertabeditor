/*
  * Copyright (C) 2014 Cameron White
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

#include "repeatindexer.h"

#include <score/score.h>
#include <score/utils.h>
#include <stack>

RepeatedSection::RepeatedSection(const SystemLocation &startBar)
    : myStartBarLocation(startBar)
{
}

bool RepeatedSection::operator<(const RepeatedSection &other) const
{
    return myStartBarLocation < other.myStartBarLocation;
}

bool RepeatedSection::hasSameStructure(const RepeatedSection &other) const
{
    // TODO - implement.
    return false;
}

void RepeatedSection::addRepeatEndBar(const SystemLocation &location,
                                      int repeatCount)
{
    myRepeatEndBars[location] = repeatCount;
}

void RepeatedSection::addAlternateEnding(int system,
                                         const AlternateEnding &ending)
{
    const SystemLocation location(system, ending.getPosition());

    // For each repeat that the ending is active, record the position
    // that should be jumped to.
    for (int num : ending.getNumbers())
        myAlternateEndings[num] = location;
}

const SystemLocation &RepeatedSection::getStartBarLocation() const
{
    return myStartBarLocation;
}

const SystemLocation &RepeatedSection::getLastEndBarLocation() const
{
    assert(!myRepeatEndBars.empty());
    return myRepeatEndBars.rbegin()->first;
}

int RepeatedSection::getAlternateEndingCount() const
{
    return static_cast<int>(myAlternateEndings.size());
}

int RepeatedSection::getTotalRepeatCount() const
{
    int count = 0;

    for (auto &bar : myRepeatEndBars)
        count += bar.second;

    return count;
}

boost::optional<SystemLocation> RepeatedSection::findAlternateEnding(int number) const
{
    if (myAlternateEndings.find(number) != myAlternateEndings.end())
        return myAlternateEndings.find(number)->second;
    else
        return boost::optional<SystemLocation>();
}

RepeatIndexer::RepeatIndexer(const Score &score)
{
    // There may be nested repeats, so maintain a stack of the active repeats
    // as we go through the score.
    std::stack<RepeatedSection> repeats;

    // The start of the score can always act as a repeat start bar.
    repeats.push(SystemLocation(0, 0));

    int systemIndex = 0;
    for (const System &system : score.getSystems())
    {
        for (const Barline &bar : system.getBarlines())
        {
            // If we've seen the last alternate ending of the repeat,
            // we are done.
            if (!repeats.empty())
            {
                RepeatedSection &activeRepeat = repeats.top();
                if (activeRepeat.getAlternateEndingCount() &&
                    activeRepeat.getAlternateEndingCount() ==
                        activeRepeat.getTotalRepeatCount())
                {
                    myRepeats.insert(activeRepeat);
                    repeats.pop();
                }
            }

            // Record any start bars that we see.
            if (bar.getBarType() == Barline::RepeatStart)
            {
                const SystemLocation location(systemIndex, bar.getPosition());
                repeats.push(RepeatedSection(location));
            }
            else if (bar.getBarType() == Barline::RepeatEnd)
            {
                // TODO - do a better job of handling mismatched repeats.
                assert(!repeats.empty());

                // Add this end bar to the active section.
                RepeatedSection &activeRepeat = repeats.top();
                activeRepeat.addRepeatEndBar(
                    SystemLocation(systemIndex, bar.getPosition()),
                    bar.getRepeatCount());

                // If we don't have any alternate endings, we must be
                // done with this repeat.
                if (activeRepeat.getAlternateEndingCount() == 0)
                {
                    myRepeats.insert(activeRepeat);
                    repeats.pop();
                }
            }

            // Process repeat endings in this bar, unless we're at the end bar.
            const Barline *nextBar = system.getNextBarline(bar.getPosition());
            if (nextBar)
            {
                for (const AlternateEnding &ending : ScoreUtils::findInRange(
                         system.getAlternateEndings(), bar.getPosition(),
                         nextBar->getPosition() - 1))
                {
                    // TODO - do a better job of handling this error.
                    assert(!repeats.empty());
                    repeats.top().addAlternateEnding(systemIndex, ending);
                }
            }
        }

        ++systemIndex;
    }
}

const RepeatedSection *RepeatIndexer::findRepeat(
    const SystemLocation &loc) const
{
    auto repeat = myRepeats.upper_bound(loc);

    // Search for a pair of start and end bars that surrounds this location.
    while (repeat != myRepeats.begin())
    {
        --repeat;
        if (repeat->getLastEndBarLocation() >= loc)
            return &(*repeat);
    }

    return nullptr;
}

boost::iterator_range<RepeatIndexer::RepeatedSectionIterator>
RepeatIndexer::getRepeats() const
{
    return boost::make_iterator_range(myRepeats);
}