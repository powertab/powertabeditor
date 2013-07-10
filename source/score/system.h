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

#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

#include <boost/serialization/access.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <vector>
#include "alternateending.h"
#include "barline.h"
#include "staff.h"
#include "tempomarker.h"

namespace Score {

class System
{
public:
    typedef std::vector<Staff>::iterator StaffIterator;
    typedef std::vector<Staff>::const_iterator StaffConstIterator;
    typedef std::vector<Barline>::iterator BarlineIterator;
    typedef std::vector<Barline>::const_iterator BarlineConstIterator;
    typedef std::vector<TempoMarker>::iterator TempoMarkerIterator;
    typedef std::vector<TempoMarker>::const_iterator TempoMarkerConstIterator;
    typedef std::vector<AlternateEnding>::iterator AlternateEndingIterator;
    typedef std::vector<AlternateEnding>::const_iterator AlternateEndingConstIterator;

    System();

    /// Returns the set of staves in the system.
    boost::iterator_range<StaffIterator> getStaves();
    /// Returns the set of staves in the system.
    boost::iterator_range<StaffConstIterator> getStaves() const;

    /// Adds a new staff to the system.
    void insertStaff(const Staff &barline);
    /// Removes the specified staff from the system.
    void removeStaff(const Staff &barline);

    /// Returns the set of barlines in the system.
    boost::iterator_range<BarlineIterator> getBarlines();
    /// Returns the set of barlines in the system.
    boost::iterator_range<BarlineConstIterator> getBarlines() const;

    /// Adds a new barline to the system.
    void insertBarline(const Barline &barline);
    /// Removes the specified barline from the system.
    void removeBarline(const Barline &barline);

    /// Returns the last barline before (or including) the given position.
    const Barline *getPreviousBarline(int position) const;
    /// Returns the first barline after (or including) the given position.
    const Barline *getNextBarline(int position) const;

    /// Returns the set of tempo markers in the system.
    boost::iterator_range<TempoMarkerIterator> getTempoMarkers();
    /// Returns the set of tempo markers in the system.
    boost::iterator_range<TempoMarkerConstIterator> getTempoMarkers() const;

    /// Adds a new tempo marker to the system.
    void insertTempoMarker(const TempoMarker &marker);
    /// Removes the specified tempo marker from the system.
    void removeTempoMarker(const TempoMarker &marker);

    /// Returns the set of alternate endings in the system.
    boost::iterator_range<AlternateEndingIterator> getAlternateEndings();
    /// Returns the set of alternate endings in system.
    boost::iterator_range<AlternateEndingConstIterator> getAlternateEndings() const;

    /// Adds a new alternate ending to the system.
    void insertAlternateEnding(const AlternateEnding &ending);
    /// Removes the specified alternate ending from the system.
    void removeAlternateEnding(const AlternateEnding &ending);

private:
    std::vector<Staff> myStaves;
    /// List of the barlines in the system. This will always contain at least
    /// two barlines - the start and end bars.
    std::vector<Barline> myBarlines;
    std::vector<TempoMarker> myTempoMarkers;
    std::vector<AlternateEnding> myAlternateEndings;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myStaves & myBarlines & myTempoMarkers & myAlternateEndings;
    }
};

}

#endif
