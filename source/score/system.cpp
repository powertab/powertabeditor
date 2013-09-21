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
#include <boost/foreach.hpp>
#include <cstddef>
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
           myChords == other.myChords;
}

boost::iterator_range<System::StaffIterator> System::getStaves()
{
    return boost::make_iterator_range(myStaves);
}

boost::iterator_range<System::StaffConstIterator> System::getStaves() const
{
    return boost::make_iterator_range(myStaves);
}

void System::insertStaff(const Staff &staff)
{
    myStaves.push_back(staff);
}

void System::removeStaff(const Staff &staff)
{
    myStaves.erase(std::remove(myStaves.begin(), myStaves.end(), staff),
                   myStaves.end());
}

boost::iterator_range<System::BarlineIterator> System::getBarlines()
{
    return boost::make_iterator_range(myBarlines);
}

boost::iterator_range<System::BarlineConstIterator> System::getBarlines() const
{
    return boost::make_iterator_range(myBarlines);
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
    BOOST_REVERSE_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() < position)
            return &barline;
    }

    return NULL;
}

const Barline *System::getNextBarline(int position) const
{
    BOOST_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() > position)
            return &barline;
    }

    return NULL;
}

boost::iterator_range<System::TempoMarkerIterator> System::getTempoMarkers()
{
    return boost::make_iterator_range(myTempoMarkers);
}

boost::iterator_range<System::TempoMarkerConstIterator> System::getTempoMarkers() const
{
    return boost::make_iterator_range(myTempoMarkers);
}

void System::insertTempoMarker(const TempoMarker &marker)
{
    ScoreUtils::insertObject(myTempoMarkers, marker);
}

void System::removeTempoMarker(const TempoMarker &marker)
{
    ScoreUtils::removeObject(myTempoMarkers, marker);
}

boost::iterator_range<System::AlternateEndingIterator> System::getAlternateEndings()
{
    return boost::make_iterator_range(myAlternateEndings);
}

boost::iterator_range<System::AlternateEndingConstIterator> System::getAlternateEndings() const
{
    return boost::make_iterator_range(myAlternateEndings);
}

void System::insertAlternateEnding(const AlternateEnding &ending)
{
    ScoreUtils::insertObject(myAlternateEndings, ending);
}

void System::removeAlternateEnding(const AlternateEnding &ending)
{
    ScoreUtils::removeObject(myAlternateEndings, ending);
}

boost::iterator_range<System::DirectionIterator> System::getDirections()
{
    return boost::make_iterator_range(myDirections);
}

boost::iterator_range<System::DirectionConstIterator> System::getDirections() const
{
    return boost::make_iterator_range(myDirections);
}

void System::insertDirection(const Direction &direction)
{
    ScoreUtils::insertObject(myDirections, direction);
}

void System::removeDirection(const Direction &direction)
{
    ScoreUtils::removeObject(myDirections, direction);
}

boost::iterator_range<System::PlayerChangeIterator> System::getPlayerChanges()
{
    return boost::make_iterator_range(myPlayerChanges);
}

boost::iterator_range<System::PlayerChangeConstIterator> System::getPlayerChanges() const
{
    return boost::make_iterator_range(myPlayerChanges);
}

void System::insertPlayerChange(const PlayerChange &change)
{
    ScoreUtils::insertObject(myPlayerChanges, change);
}

void System::removePlayerChange(const PlayerChange &change)
{
    ScoreUtils::removeObject(myPlayerChanges, change);
}

boost::iterator_range<System::ChordTextIterator> System::getChords()
{
    return boost::make_iterator_range(myChords);
}

boost::iterator_range<System::ChordTextConstIterator> System::getChords() const
{
    return boost::make_iterator_range(myChords);
}

void System::insertChord(const ChordText &chord)
{
    ScoreUtils::insertObject(myChords, chord);
}

void System::removeChord(const ChordText &chord)
{
    ScoreUtils::removeObject(myChords, chord);
}

template <typename T>
static void shift(const boost::iterator_range<T> &range, int position,
                  int offset)
{
    BOOST_FOREACH(typename T::value_type &obj, range)
    {
        if (obj.getPosition() >= position)
            obj.setPosition(obj.getPosition() + offset);
    }
}

/// Helper function for the shiftForward and shiftBackward functions.
static void shift(System &system, int position, int offset)
{
    shift(system.getBarlines(), position, offset);
    // Always keep the first bar at position 0.
    system.getBarlines().front().setPosition(0);

    shift(system.getAlternateEndings(), position, offset);
    shift(system.getTempoMarkers(), position, offset);
    shift(system.getDirections(), position, offset);
    shift(system.getPlayerChanges(), position, offset);

    BOOST_FOREACH(Staff &staff, system.getStaves())
    {
        shift(staff.getDynamics(), position, offset);

        for (int voice = 0; voice < Staff::NUM_VOICES; ++voice)
            shift(staff.getVoice(voice), position, offset);
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
