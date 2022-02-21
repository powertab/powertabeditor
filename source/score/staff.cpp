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

#include "staff.h"

#include <util/enumtostring.h>
#include "utils.h"

Staff::Staff() : myClefType(TrebleClef), myStringCount(6)
{
}

Staff::Staff(int stringCount)
    : myClefType(TrebleClef), myStringCount(stringCount)
{
}

bool Staff::operator==(const Staff &other) const
{
    return myClefType == other.myClefType &&
           myStringCount == other.myStringCount && myVoices == other.myVoices &&
           myDynamics == other.myDynamics;
}

Staff::ClefType Staff::getClefType() const
{
    return myClefType;
}

void Staff::setClefType(ClefType type)
{
    myClefType = type;
}

int Staff::getStringCount() const
{
    return myStringCount;
}

void Staff::setStringCount(int count)
{
    myStringCount = count;

    // Clean up notes / positions that are no longer valid.
    for (Voice &voice : myVoices)
    {
        for (Position &pos : voice.getPositions())
        {
            pos.removeNotes([=](const Note &note) {
                return note.getString() >= count;
            });
        }

        voice.removePositions([](const Position &pos) {
            return pos.getNotes().empty();
        });
    }
}

boost::iterator_range<Staff::VoiceIterator> Staff::getVoices()
{
    return boost::make_iterator_range(myVoices);
}

boost::iterator_range<Staff::VoiceConstIterator> Staff::getVoices() const
{
    return boost::make_iterator_range(myVoices);
}

boost::iterator_range<Staff::DynamicIterator> Staff::getDynamics()
{
    return boost::make_iterator_range(myDynamics);
}

boost::iterator_range<Staff::DynamicConstIterator> Staff::getDynamics() const
{
    return boost::make_iterator_range(myDynamics);
}

void Staff::insertDynamic(const Dynamic &dynamic)
{
    ScoreUtils::insertObject(myDynamics, dynamic);
}

void Staff::removeDynamic(const Dynamic &dynamic)
{
    ScoreUtils::removeObject(myDynamics, dynamic);
}

using ClefType = Staff::ClefType;
UTIL_DEFINE_ENUMTOSTRING(ClefType, {
    { ClefType::TrebleClef, "Treble" },
    { ClefType::BassClef, "Bass" },
})
