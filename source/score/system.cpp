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

#include <boost/foreach.hpp>

// Some helper methods to reduce code duplication.
namespace {

/// Sorts objects by their positions in the system.
template <typename T>
struct OrderByPosition
{
    bool operator()(const T &obj1, const T &obj2) const
    {
        return obj1.getPosition() < obj2.getPosition();
    }
};

template <typename T>
void insertObject(std::vector<T> &objects, const T &obj)
{
    objects.push_back(obj);
    std::sort(objects.begin(), objects.end(), OrderByPosition<T>());
}

template <typename T>
void removeObject(std::vector<T> &objects, const T &obj)
{
    objects.erase(std::remove(objects.begin(), objects.end(), obj),
                  objects.end());
}

}

namespace Score {

System::System()
{
    // Add the start and end bars.
    myBarlines.push_back(Barline());
    Barline endBar;
    endBar.setPosition(std::numeric_limits<int>::max());
    myBarlines.push_back(endBar);
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
    insertObject(myBarlines, barline);
}

void System::removeBarline(const Barline &barline)
{
    removeObject(myBarlines, barline);
}

const Barline *System::getPreviousBarline(int position) const
{
    BOOST_REVERSE_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() <= position)
            return &barline;
    }

    return NULL;
}

const Barline *System::getNextBarline(int position) const
{
    BOOST_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() >= position)
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
    insertObject(myTempoMarkers, marker);
}

void System::removeTempoMarker(const TempoMarker &marker)
{
    removeObject(myTempoMarkers, marker);
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
    insertObject(myAlternateEndings, ending);
}

void System::removeAlternateEnding(const AlternateEnding &ending)
{
    removeObject(myAlternateEndings, ending);
}

}
