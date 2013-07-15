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

#include "caret.h"

#include <boost/algorithm/clamp.hpp>
#include <score/score.h>
#include <score/system.h>

Caret::Caret(const Score &score)
    : myLocation(score)
{
}

const ScoreLocation &Caret::getLocation() const
{
    return myLocation;
}

void Caret::moveHorizontal(int offset)
{
    myLocation.setPositionIndex(boost::algorithm::clamp(
            myLocation.getPositionIndex() + offset, 0, getLastPosition()));

    onLocationChanged();
}

void Caret::moveVertical(int offset)
{
    const int numStrings = myLocation.getStaff().getStringCount();
    myLocation.setString((myLocation.getString() + offset + numStrings) %
                         numStrings);

    onLocationChanged();
}

void Caret::moveToStartPosition()
{
    myLocation.setPositionIndex(0);
    onLocationChanged();
}

void Caret::moveToEndPosition()
{
    myLocation.setPositionIndex(getLastPosition());
    onLocationChanged();
}

void Caret::moveSystem(int offset)
{
    const int prevSystem = myLocation.getSystemIndex();
    myLocation.setSystemIndex(boost::algorithm::clamp(
            myLocation.getSystemIndex() + offset, 0, getLastSystemIndex()));

    if (myLocation.getSystemIndex() != prevSystem)
    {
        myLocation.setStaffIndex(0);
        myLocation.setPositionIndex(0);
        myLocation.setString(0);

        onLocationChanged();
    }
}

void Caret::moveToFirstSystem()
{
    moveSystem(-myLocation.getSystemIndex());
}

void Caret::moveToLastSystem()
{
    moveSystem(getLastSystemIndex() - myLocation.getSystemIndex());
}

boost::signals2::connection Caret::subscribeToChanges(
        const LocationChangedSlot::slot_type &subscriber) const
{
    return onLocationChanged.connect(subscriber);
}

int Caret::getLastPosition() const
{
    // There must be at least one position space to the left of the last bar.
    return myLocation.getSystem().getBarlines().back().getPosition() - 2;
}

int Caret::getLastSystemIndex() const
{
    return myLocation.getScore().getSystems().size() - 1;
}
