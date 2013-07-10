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

#ifndef SCORE_STAFF_H
#define SCORE_STAFF_H

#include <boost/serialization/access.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <vector>
#include "dynamic.h"

namespace Score {

class Staff
{
public:
    enum ClefType
    {
        TrebleClef,
        BassClef
    };

    enum ViewType
    {
        GuitarView,
        BassView
    };

    typedef std::vector<Dynamic>::iterator DynamicIterator;
    typedef std::vector<Dynamic>::const_iterator DynamicConstIterator;

    Staff();

    bool operator==(const Staff &other) const;

    /// Returns whether the staff is shown in the guitar view, bass view, etc.
    ViewType getViewType() const;
    /// Sets which the view the staff appears in.
    void setViewType(ViewType type);

    /// Returns whether the staff is a treble or bass clef.
    ClefType getClefType() const;
    /// Sets the staff's clef type.
    void setClefType(ClefType type);

    /// Returns the number of strings in the tab staff.
    int getStringCount() const;
    /// Sets the number of strings in the tab staff.
    void setStringCount(int count);

    /// Returns the set of dynamics in the staff.
    boost::iterator_range<DynamicIterator> getDynamics();
    /// Returns the set of dynamics in the staff.
    boost::iterator_range<DynamicConstIterator> getDynamics() const;

    /// Adds a new dynamic to the staff.
    void insertDynamic(const Dynamic &dynamic);
    /// Removes the specified dynamic from the staff.
    void removeDynamic(const Dynamic &dynamic);

private:
    ViewType myViewType;
    ClefType myClefType;
    int myStringCount;
    std::vector<Dynamic> myDynamics;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myViewType & myClefType & myStringCount & myDynamics;
    }
};

}

#endif
