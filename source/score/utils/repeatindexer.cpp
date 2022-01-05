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

#include <optional>
#include <score/score.h>
#include <score/utils.h>
#include <stack>

RepeatedSection::RepeatedSection(const SystemLocation &startBar)
    : myStartBarLocation(startBar), myActiveRepeat(1)
{
}

bool RepeatedSection::operator<(const RepeatedSection &other) const
{
    return myStartBarLocation < other.myStartBarLocation;
}

void RepeatedSection::addRepeatEndBar(const SystemLocation &location,
                                      int repeatCount)
{
    myRepeatEndBars[location] = repeatCount;
}

void RepeatedSection::addAlternateEnding(const System &system, int system_index,
                                         const AlternateEnding &ending)
{
    const Barline *bar = system.getPreviousBarline(ending.getPosition() + 1);
    assert(bar);

    // For each repeat that the ending is active, record the position
    // that should be jumped to.
    const SystemLocation location(system_index, bar->getPosition());
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
    // Take the maximum of the number of alternate ending and the number of
    // repeats specified by a repeat end bar.
    int count = getAlternateEndingCount();

    for (auto &bar : myRepeatEndBars)
        count = std::max(count, bar.second);

    return count;
}

std::optional<SystemLocation> RepeatedSection::findAlternateEnding(int number) const
{
    if (myAlternateEndings.find(number) != myAlternateEndings.end())
        return myAlternateEndings.find(number)->second;
    else
        return std::optional<SystemLocation>();
}

void RepeatedSection::reset()
{
    myActiveRepeat = 1;
}

SystemLocation RepeatedSection::performRepeat(const SystemLocation &loc)
{
    // Deal with alternate endings - if we are at the start of the first
    // alternate ending, we can branch off to other alternate endings depending
    // on the active repeat.
    std::optional<SystemLocation> firstAltEnding = findAlternateEnding(1);
    if (firstAltEnding && *firstAltEnding == loc)
    {
        // Branch off to the next alternate ending, if it exists.
        std::optional<SystemLocation> nextAltEnding =
            findAlternateEnding(myActiveRepeat);
        if (nextAltEnding)
        {
            // Reset if we've left the section via the final alternate ending.
            if (myActiveRepeat == getAlternateEndingCount() &&
                *nextAltEnding >= getLastEndBarLocation())
            {
                reset();
            }

            return *nextAltEnding;
        }
    }

    // Now, we can look for repeat end bars.
    auto repeat_end = myRepeatEndBars.find(loc);

    // No repeat bar.
    if (repeat_end == myRepeatEndBars.end())
        return loc;

    int remaining_repeats = getTotalRepeatCount() - myActiveRepeat;
    if (remaining_repeats > 0)
    {
        // Perform the repeat.
        ++myActiveRepeat;
        return myStartBarLocation;
    }
    else
    {
        // Pass through the repeated section, jumping to the last repeat
        // ending. We might not be at the final repeat end bar in the section,
        // depending on where the highest alt ending number was.
        reset();
        return getLastEndBarLocation();
    }
}

static bool
needsFinalRepeatEnd(const RepeatedSection &section)
{
    int num_endings = section.getAlternateEndingCount();
    if (!num_endings)
        return false;

    // Find the last alternate ending (by position) in the section.
    // If this has anything other than the highest number, a repeat end bar is
    // required after it.
    SystemLocation last_loc(-1, -1);
    int last_num = num_endings;
    for (int i = 0; i < num_endings; ++i)
    {
        auto ending_loc = section.findAlternateEnding(i);
        if (!ending_loc)
            continue;

        if (*ending_loc >= last_loc)
        {
            last_loc = *ending_loc;
            last_num = i;
        }
    }

    if (last_num != num_endings)
    {
        // Return whether there is a missing repeat end bar after the last alt
        // ending.
        return section.getRepeatEndBars().upper_bound(last_loc) ==
               section.getRepeatEndBars().end();
    }
    else
        return false;
}

RepeatIndexer::RepeatIndexer(const Score &score)
{
    // There may be nested repeats, so maintain a stack of the active repeats
    // as we go through the score.
    std::stack<RepeatedSection> repeats;

    // The start of the score can always act as a repeat start bar.
    repeats.push(SystemLocation(0, 0));

    unsigned int systemIndex = 0;
    for (const System &system : score.getSystems())
    {
        for (const Barline &bar : system.getBarlines())
        {
            // Process repeat endings in this bar, unless we're at the last bar
            // of the system.
            const Barline *nextBar = system.getNextBarline(bar.getPosition());
            if (nextBar)
            {
                for (const AlternateEnding &ending : ScoreUtils::findInRange(
                         system.getAlternateEndings(), bar.getPosition(),
                         nextBar->getPosition() - 1))
                {
                    // TODO - report unexpected alternate endings.
                    if (!repeats.empty())
                    {
                        repeats.top().addAlternateEnding(system, systemIndex,
                                                         ending);
                    }
                }
            }

            // If we've seen the last alternate ending of the repeat,
            // we are done.
            if (!repeats.empty())
            {
                // TODO - currently, this allows a greater number of alternate
                // endings than the total repeat count so that repeats aren't
                // silently dropped. This should be reported
                // by a score checker-type feature when that is implemented.
                RepeatedSection &activeRepeat = repeats.top();
                if (activeRepeat.getAlternateEndingCount() &&
                    !activeRepeat.getRepeatEndBars().empty() &&
                    activeRepeat.getAlternateEndingCount() >=
                        activeRepeat.getTotalRepeatCount() &&
                    !needsFinalRepeatEnd(activeRepeat))
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
            // TODO - report unexpected repeat end bars.
            else if (bar.getBarType() == Barline::RepeatEnd && !repeats.empty())
            {
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
        }

        ++systemIndex;
    }

    // TODO - report mismatched repeat start bars.
    // TODO - report missing / extra alternate endings.
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

RepeatedSection *RepeatIndexer::findRepeat(
    const SystemLocation &loc)
{
    return const_cast<RepeatedSection *>(
        const_cast<const RepeatIndexer *>(this)->findRepeat(loc));
}

boost::iterator_range<RepeatIndexer::RepeatedSectionIterator>
RepeatIndexer::getRepeats() const
{
    return boost::make_iterator_range(myRepeats);
}
