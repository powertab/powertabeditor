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

#include <boost/foreach.hpp>

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

const Note *getNextNote(const Staff &staff, int voice, int position,
                        int string)
{
    const Note *note = 0;

    BOOST_FOREACH(const Position &pos, staff.getVoice(voice))
    {
        if (pos.getPosition() > position)
        {
            note = Utils::findByString(pos, string);
            break;
        }
    }

    return note;
}

const Note *getPreviousNote(const Staff &staff, int voice, int position,
                            int string)
{
    const Note *note = 0;

    BOOST_REVERSE_FOREACH(const Position &pos, staff.getVoice(voice))
    {
        if (pos.getPosition() < position)
        {
            note = Utils::findByString(pos, string);
            break;
        }
    }

    return note;
}

bool canTieNote(const Staff &staff, int voice, int position, const Note &note)
{
    const Note *prevNote = getPreviousNote(staff, voice, position,
                                           note.getString());
    return prevNote && prevNote->getFretNumber() == note.getFretNumber();
}

bool canHammerOn(const Staff &staff, int voice, int position, const Note &note)
{
    const Note *nextNote = getNextNote(staff, voice, position, note.getString());
    return nextNote && nextNote->getFretNumber() > note.getFretNumber();
}

bool canPullOff(const Staff &staff, int voice, int position, const Note &note)
{
    const Note *nextNote = getNextNote(staff, voice, position, note.getString());
    return nextNote && nextNote->getFretNumber() < note.getFretNumber();
}

}
