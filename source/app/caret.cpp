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

#include <app/pubsub/scorelocationpubsub.h>
#include <boost/algorithm/clamp.hpp>
#include <boost/make_shared.hpp>
#include <score/score.h>
#include <score/system.h>

Caret::Caret(Score &score)
    : myLocation(score),
      myInPlaybackMode(false),
      mySelectionPubSub(boost::make_shared<ScoreLocationPubSub>())
{
    mySelectionPubSub->subscribe(
                boost::bind(&Caret::handleSelectionChanged, this, _1));
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
    myLocation.setStaffIndex(boost::algorithm::clamp(
            myLocation.getStaffIndex() + offset, 0,
            myLocation.getSystem().getStaves().size() - 1));

    onLocationChanged();
}

void Caret::moveToNextBar()
{
    const Barline *nextBar = myLocation.getSystem().getNextBarline(
                myLocation.getPositionIndex());
    if (!nextBar)
        return;

    // Move into the next system if necessary.
    if (*nextBar == myLocation.getSystem().getBarlines().back())
        moveToSystem(myLocation.getSystemIndex() + 1, true);
    else
        moveToPosition(nextBar->getPosition() + 1);
}

void Caret::moveToPrevBar()
{
    const System &system = myLocation.getSystem();
    const Barline *prevBar = system.getPreviousBarline(
                myLocation.getPositionIndex());
    if (prevBar)
        prevBar = system.getPreviousBarline(prevBar->getPosition());

    if (prevBar)
    {
        if (*prevBar == system.getBarlines().front())
            moveToStartPosition();
        else
            moveToPosition(prevBar->getPosition() + 1);
    }
    // Move up by a system if we're at the start of our current system.
    else if (myLocation.getSystemIndex() > 0)
    {
        moveToSystem(myLocation.getSystemIndex() - 1, true);

        // Move to the last barline if possible.
        const System &newSystem = myLocation.getSystem();
        const size_t count = newSystem.getBarlines().size();
        if (count > 2)
            moveToPosition(newSystem.getBarlines()[count - 2].getPosition() + 1);
    }
}

boost::signals2::connection Caret::subscribeToChanges(
        const LocationChangedSlot::slot_type &subscriber) const
{
    return onLocationChanged.connect(subscriber);
}

boost::shared_ptr<ScoreLocationPubSub> Caret::getSelectionPubSub() const
{
    return mySelectionPubSub;
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

void Caret::moveToPosition(int position)
{
    myLocation.setPositionIndex(boost::algorithm::clamp(position, 0,
                                                        getLastPosition()));
    myLocation.setSelectionStart(myLocation.getPositionIndex());

    onLocationChanged();
}

void Caret::moveToSystem(int system, bool keepStaff)
{
    const int prevSystem = myLocation.getSystemIndex();
    myLocation.setSystemIndex(boost::algorithm::clamp(system, 0,
                                                      getLastSystemIndex()));

    if (myLocation.getSystemIndex() != prevSystem)
    {
        if (!keepStaff)
            myLocation.setStaffIndex(0);
        else
        {
            myLocation.setStaffIndex(boost::algorithm::clamp(
                    myLocation.getStaffIndex(), 0,
                    myLocation.getSystem().getStaves().size()));
        }

        myLocation.setPositionIndex(0);
        myLocation.setSelectionStart(0);
        myLocation.setString(0);

        onLocationChanged();
    }
}

void Caret::handleSelectionChanged(const ScoreLocation &location)
{
    // Ignore mouse clicks while in playback mode.
    if (myInPlaybackMode)
        return;

    myLocation.setSystemIndex(location.getSystemIndex());
    myLocation.setStaffIndex(location.getStaffIndex());
    myLocation.setPositionIndex(location.getPositionIndex());
    myLocation.setSelectionStart(location.getSelectionStart());
    myLocation.setString(location.getString());

    onLocationChanged();
}
