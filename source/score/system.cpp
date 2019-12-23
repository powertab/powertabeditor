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
#include <util/iteratorrange_reverse.h>
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

Util::IteratorRange<System::StaffIterator> System::getStaves()
{
    return myStaves;
}

Util::IteratorRange<System::StaffConstIterator> System::getStaves() const
{
    return myStaves;
}

void System::insertStaff(const Staff &staff)
{
    myStaves.push_back(staff);
}

void System::insertStaff(const Staff &staff, int index)
{
    myStaves.insert(myStaves.begin() + index, staff);
}

void System::removeStaff(int index)
{
    myStaves.erase(myStaves.begin() + index);
}

Util::IteratorRange<System::BarlineIterator> System::getBarlines()
{
    return myBarlines;
}

Util::IteratorRange<System::BarlineConstIterator> System::getBarlines() const
{
    return myBarlines;
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
    for (const Barline &barline : Util::reverseRange(myBarlines))
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

Util::IteratorRange<System::TempoMarkerIterator> System::getTempoMarkers()
{
    return myTempoMarkers;
}

Util::IteratorRange<System::TempoMarkerConstIterator> System::getTempoMarkers() const
{
    return myTempoMarkers;
}

void System::insertTempoMarker(const TempoMarker &marker)
{
    ScoreUtils::insertObject(myTempoMarkers, marker);
}

void System::removeTempoMarker(const TempoMarker &marker)
{
    ScoreUtils::removeObject(myTempoMarkers, marker);
}

Util::IteratorRange<System::AlternateEndingIterator> System::getAlternateEndings()
{
    return myAlternateEndings;
}

Util::IteratorRange<System::AlternateEndingConstIterator> System::getAlternateEndings() const
{
    return myAlternateEndings;
}

void System::insertAlternateEnding(const AlternateEnding &ending)
{
    ScoreUtils::insertObject(myAlternateEndings, ending);
}

void System::removeAlternateEnding(const AlternateEnding &ending)
{
    ScoreUtils::removeObject(myAlternateEndings, ending);
}

Util::IteratorRange<System::DirectionIterator> System::getDirections()
{
    return myDirections;
}

Util::IteratorRange<System::DirectionConstIterator> System::getDirections() const
{
    return myDirections;
}

void System::insertDirection(const Direction &direction)
{
    ScoreUtils::insertObject(myDirections, direction);
}

void System::removeDirection(const Direction &direction)
{
    ScoreUtils::removeObject(myDirections, direction);
}

Util::IteratorRange<System::PlayerChangeIterator> System::getPlayerChanges()
{
    return myPlayerChanges;
}

Util::IteratorRange<System::PlayerChangeConstIterator> System::getPlayerChanges() const
{
    return myPlayerChanges;
}

void System::insertPlayerChange(const PlayerChange &change)
{
    ScoreUtils::insertObject(myPlayerChanges, change);
}

void System::removePlayerChange(const PlayerChange &change)
{
    ScoreUtils::removeObject(myPlayerChanges, change);
}

Util::IteratorRange<System::ChordTextIterator> System::getChords()
{
    return myChords;
}

Util::IteratorRange<System::ChordTextConstIterator> System::getChords() const
{
    return myChords;
}

void System::insertChord(const ChordText &chord)
{
    ScoreUtils::insertObject(myChords, chord);
}

void System::removeChord(const ChordText &chord)
{
    ScoreUtils::removeObject(myChords, chord);
}

Util::IteratorRange<System::TextItemIterator> System::getTextItems()
{
    return myTextItems;
}

Util::IteratorRange<System::TextItemConstIterator> System::getTextItems() const
{
    return myTextItems;
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
