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

#include "system.h"

#include <algorithm>
#include <cstddef>
#include <ranges>
#include "utils.h"

System::System()
{
    // Add the start and end bars.
    myBarlines.push_back(Barline());
    Barline endBar;
    endBar.setPosition(30);
    myBarlines.push_back(endBar);
}

bool System::operator==(const System &other) const
{
    return myStaves == other.myStaves && myBarlines == other.myBarlines &&
           myTempoMarkers == other.myTempoMarkers &&
           myAlternateEndings == other.myAlternateEndings &&
           myDirections == other.myDirections &&
           myPlayerChanges == other.myPlayerChanges &&
           myChords == other.myChords &&
           myTextItems == other.myTextItems;
}

void System::insertStaff(const Staff &staff)
{
    myStaves.push_back(staff);
}

void System::insertStaff(Staff &&staff)
{
    myStaves.push_back(std::move(staff));
}

void System::insertStaff(const Staff &staff, int index)
{
    myStaves.insert(myStaves.begin() + index, staff);
}

void System::removeStaff(int index)
{
    myStaves.erase(myStaves.begin() + index);
}

void System::insertBarline(const Barline &barline)
{
    // Ensure that the end bar remains the end bar.
    myBarlines.back().setPosition(
        std::max(myBarlines.back().getPosition(), barline.getPosition() + 1));
    ScoreUtils::insertObject(myBarlines, barline);
}

void System::removeBarline(const Barline &barline)
{
    ScoreUtils::removeObject(myBarlines, barline);
}

const Barline *System::getPreviousBarline(int position) const
{
    for (const Barline &barline : std::views::reverse(myBarlines))
    {
        if (barline.getPosition() < position)
            return &barline;
    }

    return nullptr;
}

const Barline *System::getNextBarline(int position) const
{
    for (const Barline &barline : myBarlines)
    {
        if (barline.getPosition() > position)
            return &barline;
    }

    return nullptr;
}

Barline *System::getNextBarline(int position)
{
    for (Barline &barline : myBarlines)
    {
        if (barline.getPosition() > position)
            return &barline;
    }

    return nullptr;
}

void System::insertTempoMarker(const TempoMarker &marker)
{
    ScoreUtils::insertObject(myTempoMarkers, marker);
}

void System::removeTempoMarker(const TempoMarker &marker)
{
    ScoreUtils::removeObject(myTempoMarkers, marker);
}

void System::insertAlternateEnding(const AlternateEnding &ending)
{
    ScoreUtils::insertObject(myAlternateEndings, ending);
}

void System::removeAlternateEnding(const AlternateEnding &ending)
{
    ScoreUtils::removeObject(myAlternateEndings, ending);
}

void System::insertDirection(const Direction &direction)
{
    ScoreUtils::insertObject(myDirections, direction);
}

void System::removeDirection(const Direction &direction)
{
    ScoreUtils::removeObject(myDirections, direction);
}

void System::insertPlayerChange(const PlayerChange &change)
{
    ScoreUtils::insertObject(myPlayerChanges, change);
}

void System::removePlayerChange(const PlayerChange &change)
{
    ScoreUtils::removeObject(myPlayerChanges, change);
}

void System::insertChord(const ChordText &chord)
{
    ScoreUtils::insertObject(myChords, chord);
}

void System::removeChord(const ChordText &chord)
{
    ScoreUtils::removeObject(myChords, chord);
}

void System::insertTextItem(const TextItem &text)
{
    ScoreUtils::insertObject(myTextItems, text);
}

void System::removeTextItem(const TextItem &text)
{
    ScoreUtils::removeObject(myTextItems, text);
}

template <typename T>
static void shift(const T &range, int position,
                  int offset)
{
    for (auto &obj : range)
    {
        if (obj.getPosition() >= position)
            obj.setPosition(obj.getPosition() + offset);
    }
}

void SystemUtils::shift(System &system, int position, int offset)
{
    shift(system.getBarlines(), position, offset);
    // Always keep the first bar at position 0.
    system.getBarlines().front().setPosition(0);

    shift(system.getAlternateEndings(), position, offset);
    shift(system.getTempoMarkers(), position, offset);
    shift(system.getDirections(), position, offset);
    shift(system.getPlayerChanges(), position, offset);
    shift(system.getChords(), position, offset);
    shift(system.getTextItems(), position, offset);

    for (Staff &staff : system.getStaves())
    {
        shift(staff.getDynamics(), position, offset);

        for (Voice &voice : staff.getVoices())
        {
            shift(voice.getPositions(), position, offset);
            shift(voice.getIrregularGroupings(), position, offset);
        }
    }
}

void SystemUtils::shiftForward(System &system, int position)
{
    shift(system, position, 1);
}

void SystemUtils::shiftBackward(System &system, int position)
{
    shift(system, position, -1);
}

std::pair<const Barline &, const Barline &>
SystemUtils::getSurroundingBarlines(const System &system, int position)
{
    // If we're exactly on top of a barline, use that instead of grabbing the
    // preceding barline.
    const Barline *current_bar = ScoreUtils::findByPosition(
        system.getBarlines(), position);
    if (!current_bar)
        current_bar = system.getPreviousBarline(position);

    const Barline *next_bar = system.getNextBarline(position);
    assert(next_bar);

    return { *current_bar, *next_bar };
}
