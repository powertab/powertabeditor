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

#include "editstaff.h"

#include <score/score.h>

EditStaff::EditStaff(const ScoreLocation &location, Staff::ClefType clef,
    int strings)
    : QUndoCommand(tr("Edit Staff")),
    myLocation(location),
    myClef(clef),
    myNumStrings(strings)
{
}

void EditStaff::redo()
{
    System &system = myLocation.getSystem();
    myOriginalSystem = system;

    Staff &staff = myLocation.getStaff();
    staff.setClefType(myClef);

    // If we're changing the number of strings, more work is required...
    if (myNumStrings != staff.getStringCount())
    {
        Score &score = myLocation.getScore();
        const int staff_index = myLocation.getStaffIndex();

        // If the following system doesn't start with a player change, it will
        // need one so that it has players with the correct number of strings.
        const int next_system_index = myLocation.getSystemIndex() + 1;
        if (next_system_index < static_cast<int>(score.getSystems().size()))
        {
            System &next_system = score.getSystems()[next_system_index];
            myOriginalNextSystem = next_system;

            if (static_cast<int>(next_system.getStaves().size()) >= staff_index)
                addPlayerChangeAtStart(score, next_system_index);
        }

        // Ensure that there's a player change at the start of the system.
        addPlayerChangeAtStart(score, myLocation.getSystemIndex());

        // Clear out all active players in this staff.
        for (PlayerChange &change : system.getPlayerChanges())
        {
            for (const ActivePlayer &p : change.getActivePlayers(staff_index))
                change.removeActivePlayer(staff_index, p);
        }

        staff.setStringCount(myNumStrings);
    }
}

void EditStaff::undo()
{
    Score &score = myLocation.getScore();
    const int system_index = myLocation.getSystemIndex();
    score.getSystems()[system_index] = myOriginalSystem;

    if (myOriginalNextSystem)
        score.getSystems()[system_index + 1] = *myOriginalNextSystem;
}

void EditStaff::addPlayerChangeAtStart(Score &score, int system_index)
{
    System &system = score.getSystems()[system_index];
    const PlayerChange *current_players =
        ScoreUtils::getCurrentPlayers(score, system_index, 0);

    if (current_players &&
        (system.getPlayerChanges().empty() ||
         system.getPlayerChanges().front().getPosition() != 0))
    {
        PlayerChange change(*current_players);
        change.setPosition(0);
        system.insertPlayerChange(change);
    }
}
