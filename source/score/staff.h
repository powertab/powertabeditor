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

#include <array>
#include <boost/range/iterator_range_core.hpp>
#include "dynamic.h"
#include "fileversion.h"
#include <util/enumtostring_fwd.h>
#include <vector>
#include "voice.h"

class Staff
{
public:
    /// Number of voices in a staff.
    static inline constexpr int NUM_VOICES = 2;

    enum ClefType
    {
        TrebleClef,
        BassClef
    };

    typedef std::array<Voice, NUM_VOICES> VoiceList;
    typedef VoiceList::iterator VoiceIterator;
    typedef VoiceList::const_iterator VoiceConstIterator;
    typedef std::vector<Dynamic>::iterator DynamicIterator;
    typedef std::vector<Dynamic>::const_iterator DynamicConstIterator;

    Staff();
    explicit Staff(int stringCount);

    bool operator==(const Staff &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns whether the staff is a treble or bass clef.
    ClefType getClefType() const;
    /// Sets the staff's clef type.
    void setClefType(ClefType type);

    /// Returns the number of strings in the tab staff.
    int getStringCount() const;
    /// Sets the number of strings in the tab staff.
    /// If the number of strings is being reduced, some notes may be removed.
    void setStringCount(int count);

    /// Returns the voices in the staff.
    boost::iterator_range<VoiceIterator> getVoices();
    /// Returns the voices in the staff.
    boost::iterator_range<VoiceConstIterator> getVoices() const;

    /// Returns the set of dynamics in the staff.
    boost::iterator_range<DynamicIterator> getDynamics();
    /// Returns the set of dynamics in the staff.
    boost::iterator_range<DynamicConstIterator> getDynamics() const;

    /// Adds a new dynamic to the staff.
    void insertDynamic(const Dynamic &dynamic);
    /// Removes the specified dynamic from the staff.
    void removeDynamic(const Dynamic &dynamic);

private:
    ClefType myClefType;
    int myStringCount;
    std::array<Voice, NUM_VOICES> myVoices;
    std::vector<Dynamic> myDynamics;
};

UTIL_DECLARE_ENUMTOSTRING(Staff::ClefType)

template <class Archive>
void Staff::serialize(Archive &ar, const FileVersion version)
{
    if (version < FileVersion::VIEW_FILTERS)
    {
        int view_type = 0;
        ar("view_type", view_type);
    }

    ar("clef_type", myClefType);
    ar("string_count", myStringCount);
    ar("voices", myVoices);
    ar("dynamics", myDynamics);
}

#endif
