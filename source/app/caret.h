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

#ifndef APP_CARET_H
#define APP_CARET_H

#include <boost/signals2/signal.hpp>
#include <score/scorelocation.h>
#include <painters/scoreclickevent.h>

class ViewOptions;

/// Tracks the current location within the score.
class Caret
{
public:
    Caret(Score &score, const ViewOptions &options);

    ScoreLocation &getLocation();
    const ScoreLocation &getLocation() const;

    /// @{
    /// The currently selected score item. If nothing is selected, the staff is
    /// "selected".
    ScoreItem getSelectedItem() const;
    void setSelectedItem(ScoreItem item);
    /// @}

    /// Returns whether the caret is in playback mode.
    bool isInPlaybackMode() const;
    /// Sets whether the caret is in playback mode.
    void setIsInPlaybackMode(bool set);

    /// Moves the caret left or right by the specified offset.
    void moveHorizontal(int offset);

    /// Moves the caret up or down to the next string, and wraps around
    /// if necessary.
    void moveVertical(int offset);

    /// Moves the caret to the first position in the staff.
    void moveToStartPosition();

    /// Moves the caret to the last position in the staff.
    void moveToEndPosition();

    /// Move to the given position index.
    void moveToPosition(int position);

    /// Moves the caret up/down by the given number of systems.
    void moveSystem(int offset);

    /// Moves the caret to the first system in the score.
    void moveToFirstSystem();

    /// Moves the caret to the last system in the score.
    void moveToLastSystem();

    /// Move to the given system index, optionally keeping the same staff index.
    bool moveToSystem(int system, bool keepStaff);

    /// Moves up or down by the given number of staves.
    void moveStaff(int offset);

    /// Moves the caret to the next bar after the current position, or to the
    /// next system if necessary.
    bool moveToNextBar();

    /// Moves the caret to the bar before the current position, or to the
    /// previous system if necessary.
    void moveToPrevBar();

    /// Moves to the specified location.
    void moveToLocation(const ConstScoreLocation &location);

    /// Ensures that the caret is still at a valid position.
    void moveToValidPosition();

    typedef boost::signals2::signal<void ()> LocationChangedSlot;
    boost::signals2::connection subscribeToChanges(
            const LocationChangedSlot::slot_type &subscriber) const;

private:
    /// Move to the specified staff.
    void moveToStaff(int staff);

    /// Returns the last valid position in the system (not including the end bar).
    int getLastPosition() const;
    /// Returns the last valid system index in the score.
    int getLastSystemIndex() const;

    ScoreLocation myLocation;
    ScoreItem mySelectedItem = ScoreItem::Staff;
    const ViewOptions &myViewOptions;
    bool myInPlaybackMode;

    /// Send out signals to subscribers whenever the location changes.
    mutable LocationChangedSlot onLocationChanged;
};

#endif
