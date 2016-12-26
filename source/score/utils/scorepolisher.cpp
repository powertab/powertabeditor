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

#include "scorepolisher.h"

#include <map>
#include <score/score.h>
#include <score/voiceutils.h>
#include <score/utils.h>
#include <unordered_map>
#include <unordered_set>

class TimeStamp
{
public:
    bool operator<(const TimeStamp &other) const
    {
        if (myTime == other.myTime)
        {
            // Order the timestamps so that grace notes appear before the actual
            // note.
            return myGraceNoteNumber.get_value_or(
                       std::numeric_limits<int>::max()) <
                   other.myGraceNoteNumber.get_value_or(
                       std::numeric_limits<int>::max());
        }
        else
            return myTime < other.myTime;
    }

    void advance(const boost::rational<int> &duration)
    {
        myTime += duration;
    }

    void setGraceNoteNumber(boost::optional<int> count)
    {
        myGraceNoteNumber = count;
    }

private:
    /// The time from the start of the bar.
    boost::rational<int> myTime;
    /// Grace notes occur at the same timestamp as the note that they precede,
    /// but need to appear before the actual note.
    boost::optional<int> myGraceNoteNumber;
};

static int getDefaultNoteSpacing(const boost::rational<int> &duration)
{
    return std::max(2 * boost::rational_cast<int>(duration), 1);
}

template <typename T>
static void shiftItemsAtPosition(const T &items, int position, int newPosition,
                                 std::unordered_set<const void *> &knownItems)
{
    for (auto &item : ScoreUtils::findInRange(items, position, position))
    {
        if (knownItems.find(&item) != knownItems.end())
            continue;

        knownItems.insert(&item);
        item.setPosition(newPosition);
    }
}

static void shiftAllItemsAtPosition(
    System &system, Staff &staff, Voice &voice, int currentPosition,
    int newPosition, std::unordered_set<const void *> &knownItems)
{
    shiftItemsAtPosition(voice.getIrregularGroupings(), currentPosition,
                         newPosition, knownItems);
    shiftItemsAtPosition(staff.getDynamics(), currentPosition, newPosition,
                         knownItems);
    shiftItemsAtPosition(system.getTextItems(), currentPosition, newPosition,
                         knownItems);
    shiftItemsAtPosition(system.getChords(), currentPosition, newPosition,
                         knownItems);
    shiftItemsAtPosition(system.getTempoMarkers(), currentPosition, newPosition,
                         knownItems);
    shiftItemsAtPosition(system.getDirections(), currentPosition, newPosition,
                         knownItems);
    shiftItemsAtPosition(system.getPlayerChanges(), currentPosition,
                         newPosition, knownItems);
    shiftItemsAtPosition(system.getAlternateEndings(), currentPosition,
                         newPosition, knownItems);
}

static void computeTimestampPosition(
    const TimeStamp &timestamp, int minPosition,
    std::map<TimeStamp, int> &timestampPositions)
{
    int position = 0;

    // If another voice has a note at this timestamp, use that position.
    auto it = timestampPositions.find(timestamp);
    if (it != timestampPositions.end())
        position = std::max(it->second, minPosition);
    else
    {
        // If this timestamp falls in between two timestamps from another voice,
        // insert it and shift the following timestamps over if necessary.
        it = timestampPositions.lower_bound(timestamp);

        if (it != timestampPositions.begin())
            position = std::max(boost::prior(it)->second + 1, minPosition);
        else
            position = 0;

        if (it != timestampPositions.end() && it->second <= position)
        {
            const int shiftAmount = (position - it->second) + 1;
            for (; it != timestampPositions.end(); ++it)
                it->second += shiftAmount;
        }
    }

    timestampPositions[timestamp] = position;
}

void ScoreUtils::polishSystem(System &system)
{
    // Format each bar separately.
    for (Barline &leftBar : system.getBarlines())
    {
        Barline *rightBar = system.getNextBarline(leftBar.getPosition());
        if (!rightBar)
            break;

        std::unordered_map<const Position *, TimeStamp> timestamps;
        std::map<TimeStamp, int> timestampPositions;

        // For each timestamp, compute the maximum position at that timestamp
        // for any staff.
        for (const Staff &staff : system.getStaves())
        {
            for (const Voice &voice : staff.getVoices())
            {
                TimeStamp timestamp;
                boost::optional<int> grace_note;
                int currentPosition = 0;

                for (const Position &position : ScoreUtils::findInRange(
                         voice.getPositions(), leftBar.getPosition(),
                         rightBar->getPosition()))
                {
                    if (position.hasProperty(Position::Acciaccatura))
                        grace_note = grace_note.get_value_or(0) + 1;
                    else
                        grace_note.reset();

                    timestamp.setGraceNoteNumber(grace_note);

                    computeTimestampPosition(timestamp, currentPosition,
                                             timestampPositions);
                    boost::rational<int> duration =
                        VoiceUtils::getDurationTime(voice, position);

                    currentPosition = timestampPositions[timestamp] +
                                      getDefaultNoteSpacing(duration);
                    timestamps[&position] = timestamp;
                    timestamp.advance(duration);
                }

                // Track where the right barline should be.
                computeTimestampPosition(timestamp, currentPosition,
                                         timestampPositions);
            }
        }

        // Empty bar - leave as-is.
        if (timestampPositions.empty())
            continue;

        int maxPosition = std::max(1, timestampPositions.rbegin()->second);

        // Adjust!
        const int startPos =
            (leftBar.getPosition() == 0) ? 0 : leftBar.getPosition() + 1;
        const int oldEndPos = rightBar->getPosition();
        const int endPos = startPos + maxPosition;
        std::unordered_set<const void *> knownItems;

        if (endPos > oldEndPos)
        {
            SystemUtils::shift(system, rightBar->getPosition(),
                               endPos - rightBar->getPosition());
        }
        else
        {
            for (Staff &staff : system.getStaves())
            {
                for (Voice &voice : staff.getVoices())
                {
                    shiftAllItemsAtPosition(system, staff, voice, oldEndPos,
                                            endPos, knownItems);
                }
            }
            rightBar->setPosition(endPos);
        }

        for (Staff &staff : system.getStaves())
        {
            for (Voice &voice : staff.getVoices())
            {
                for (Position &pos :
                     ScoreUtils::findInRange(voice.getPositions(),
                                             leftBar.getPosition(), oldEndPos))
                {
                    // Since we're moving around irregular groups, we need to
                    // have precomputed the durations of each position.
                    TimeStamp timestamp = timestamps[&pos];
                    const int currentPosition = pos.getPosition();
                    const int newPosition =
                        startPos + timestampPositions[timestamp];

                    // Move any irregular groups, etc that start at this
                    // position. If the group moves forward, we need to be
                    // careful not to try to move it again on a later iteration.
                    shiftAllItemsAtPosition(system, staff, voice,
                                            currentPosition, newPosition,
                                            knownItems);

                    pos.setPosition(newPosition);
                }
            }
        }
    }
}

void ScoreUtils::polishScore(Score &score)
{
    for (System &system : score.getSystems())
        polishSystem(system);
}
