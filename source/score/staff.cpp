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

#include "utils.h"

Staff::Staff()
    : myViewType(GuitarView),
      myClefType(TrebleClef),
      myStringCount(6)
{
    // boost::array does not initialize elements.
    myVoices.assign(std::vector<Position>());
}

bool Staff::operator==(const Staff &other) const
{
    return myViewType == other.myViewType &&
           myClefType == other.myClefType &&
           myStringCount == other.myStringCount &&
           myVoices == other.myVoices &&
           myDynamics == other.myDynamics;
}

Staff::ViewType Staff::getViewType() const
{
    return myViewType;
}

void Staff::setViewType(ViewType type)
{
    myViewType = type;
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
}

boost::iterator_range<Staff::VoiceIterator> Staff::getVoice(int voice)
{
    return boost::make_iterator_range(myVoices.at(voice));
}

boost::iterator_range<Staff::VoiceConstIterator> Staff::getVoice(
        int voice) const
{
    return boost::make_iterator_range(myVoices.at(voice));
}

void Staff::insertPosition(int voice, const Position &position)
{
    ScoreUtils::insertObject(myVoices.at(voice), position);
}

void Staff::removePosition(int voice, const Position &position)
{
    ScoreUtils::removeObject(myVoices.at(voice), position);
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
