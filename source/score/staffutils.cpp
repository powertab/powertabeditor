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

#include "staffutils.h"

namespace StaffUtils {

FilteredVoiceConstIterator getPositionsInRange(const Staff &staff, int voice,
                                               int left, int right)
{
    boost::iterator_range<Staff::VoiceConstIterator> range = staff.getVoice(voice);
    return boost::adaptors::filter(range, InPositionRange(left, right));
}

InPositionRange::InPositionRange(int left, int right)
    : myLeft(left), myRight(right)
{
}

bool InPositionRange::operator ()(const Position &pos) const
{
    return pos.getPosition() >= myLeft && pos.getPosition() <= myRight;
}

}
