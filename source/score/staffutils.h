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

#ifndef SCORE_STAFFUTILS_H
#define SCORE_STAFFUTILS_H

#include <boost/range/adaptor/filtered.hpp>
#include "staff.h"

namespace StaffUtils {

struct InPositionRange
{
    InPositionRange(int left, int right);

    bool operator()(const Position &pos) const;

private:
    const int myLeft;
    const int myRight;
};

typedef boost::filtered_range<InPositionRange,
                              boost::iterator_range<Staff::VoiceConstIterator> >
        FilteredVoiceConstIterator;

/// Returns the specified range of positions in a voice.
FilteredVoiceConstIterator getPositionsInRange(const Staff &staff, int voice,
                                               int left, int right);

/// Finds the next note in the staff on the given string.
const Note *getNextNote(const Staff &staff, int voice, int position,
                        int string);

/// Finds the previous note in the staff on the given string.
const Note *getPreviousNote(const Staff &staff, int voice, int position,
                            int string);
}

#endif
