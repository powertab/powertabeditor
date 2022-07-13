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

#include <app/viewoptions.h>
#include <score/score.h>
#include <score/system.h>

#include <algorithm>

Caret::Caret(Score &score, const ViewOptions &options)
    : myLocation(score), myViewOptions(options), myInPlaybackMode(false)
{
}

ScoreLocation &Caret::getLocation()
{
    return myLocation;
}

const ScoreLocation &Caret::getLocation() const
{
    return myLocation;
}

bool Caret::isInPlaybackMode() const
{
    return myInPlaybackMode;
}

void Caret::setIsInPlaybackMode(bool set)
{
    myInPlaybackMode = set;
    // Trigger a redraw.
    onLocationChanged();
}

void Caret::moveHorizontal(int offset)
{
    moveToPosition(myLocation.getPositionIndex() + offset);
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
    moveToPosition(0);
}

void Caret::moveToEndPosition()
{
    moveToPosition(getLastPosition());
}

void Caret::moveSystem(int offset)
{
    moveToSystem(myLocation.getSystemIndex() + offset, false);
}

void Caret::moveToFirstSystem()
{
    moveToSystem(0, false);
}

void Caret::moveToLastSystem()
{
    moveToSystem(getLastSystemIndex(), false);
}

void Caret::moveStaff(int offset)
{
    moveToStaff(myLocation.getStaffIndex() + offset);
}

void Caret::moveToStaff(int staff)
{
    const int num_staves =
        static_cast<int>(myLocation.getSystem().getStaves().size());
    staff = std::clamp(staff, 0, num_staves - 1);

    const bool is_increasing = staff >= myLocation.getStaffIndex();
    const int increment = is_increasing ? 1 : -1;
    const int end = is_increasing ? num_staves : -1;

    const Score &score = myLocation.getScore();
    const ViewFilter *filter =
        myViewOptions.getFilter()
            ? &score.getViewFilters()[*myViewOptions.getFilter()]
            : nullptr;

    // If the specified staff is hidden by the current filter, try the staves
    // before or after in that direction.
    for (int i = staff; i != end; i += increment)
    {
        if (!filter || filter->accept(score, myLocation.getSystemIndex(), i))
        {
            myLocation.setStaffIndex(i);
            onLocationChanged();
            return;
        }
    }
}

bool Caret::moveToNextBar()
{
    const Barline *nextBar = myLocation.getSystem().getNextBarline(
                myLocation.getPositionIndex());
    if (!nextBar)
        return false;

    // Move into the next system if necessary.
    if (*nextBar == myLocation.getSystem().getBarlines().back())
        return moveToSystem(myLocation.getSystemIndex() + 1, true);
    else
    {
        moveToPosition(nextBar->getPosition());
        return true;
    }
}

void Caret::moveToPrevBar()
{
    const System &system = myLocation.getSystem();
    const Barline *prevBar = system.getPreviousBarline(
                myLocation.getPositionIndex());
    if (prevBar)
        prevBar = system.getPreviousBarline(prevBar->getPosition());

    if (prevBar)
        moveToPosition(prevBar->getPosition());
    else if (myLocation.getSystemIndex() > 0)
    {
        // Move up by a system if we're at the start of our current system.
        moveToSystem(myLocation.getSystemIndex() - 1, true);

        // Move to the last barline if possible.
        const System &newSystem = myLocation.getSystem();
        const size_t count = newSystem.getBarlines().size();
        if (count > 2)
            moveToPosition(newSystem.getBarlines()[count - 2].getPosition());
    }
}

boost::signals2::connection Caret::subscribeToChanges(
        const LocationChangedSlot::slot_type &subscriber) const
{
    return onLocationChanged.connect(subscriber);
}

int Caret::getLastPosition() const
{
    // There must be at least one position space to the left of the last bar.
    return myLocation.getSystem().getBarlines().back().getPosition() - 1;
}

int Caret::getLastSystemIndex() const
{
    return static_cast<int>(myLocation.getScore().getSystems().size() - 1);
}

void Caret::moveToPosition(int position)
{
    // Allow moving to the last barline's position (inclusive)
    int last_pos = myLocation.getSystem().getBarlines().back().getPosition();
    myLocation.setPositionIndex(std::clamp(position, 0, last_pos));
    myLocation.setSelectionStart(myLocation.getPositionIndex());

    onLocationChanged();
}

bool Caret::moveToSystem(int system, bool keepStaff)
{
    const int prevSystem = myLocation.getSystemIndex();
    myLocation.setSystemIndex(std::clamp(system, 0, getLastSystemIndex()));

    if (myLocation.getSystemIndex() != prevSystem)
    {
        if (!keepStaff)
            myLocation.setStaffIndex(0);
        else
        {
            myLocation.setStaffIndex(
                std::clamp(myLocation.getStaffIndex(), 0,
                           static_cast<int>(
                               myLocation.getSystem().getStaves().size() - 1)));
        }

        myLocation.setPositionIndex(0);
        myLocation.setSelectionStart(0);
        myLocation.setString(0);

        onLocationChanged();
        return true;
    }
    else
        return false;
}

void Caret::moveToLocation(const ConstScoreLocation &location)
{
    myLocation.setSystemIndex(location.getSystemIndex());
    myLocation.setStaffIndex(location.getStaffIndex());
    myLocation.setPositionIndex(location.getPositionIndex());
    myLocation.setSelectionStart(location.getSelectionStart());
    myLocation.setString(location.getString());

    onLocationChanged();
}

void Caret::moveToValidPosition()
{
    moveToSystem(myLocation.getSystemIndex(), true);
    moveToStaff(myLocation.getStaffIndex());
    moveToPosition(myLocation.getPositionIndex());
}

ScoreItem
Caret::getSelectedItem() const
{
    return mySelectedItem;
}

void
Caret::setSelectedItem(ScoreItem item)
{
    mySelectedItem = item;
}
