/*
 * Copyright (C) 2020 Cameron White
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

#include "shiftstring.h"

#include <score/score.h>
#include <score/utils.h>

ShiftString::ShiftString(const ScoreLocation &location, bool shift_up)
    : QUndoCommand(shift_up ? QObject::tr("Shift String Up")
                            : QObject::tr("Shift String Down")),
      myLocation(location),
      myShiftUp(shift_up)
{
}

static const Tuning *
findActiveTuning(const ScoreLocation &location)
{
    const PlayerChange *player_change = ScoreUtils::getCurrentPlayers(
        location.getScore(), location.getSystemIndex(),
        location.getPositionIndex());
    if (!player_change)
        return nullptr;

    std::vector<ActivePlayer> active_players =
        player_change->getActivePlayers(location.getStaffIndex());
    if (active_players.empty())
        return nullptr;

    const int player_idx = active_players.front().getPlayerNumber();
    const Player &player = location.getScore().getPlayers()[player_idx];
    return &player.getTuning();
}

static void
shiftNote(Note &note, const Tuning &tuning, bool shift_up)
{
    // TODO - check bounds.
    const int new_string = note.getString() + (shift_up ? -1 : 1);
    const int fret_offset = tuning.getNote(note.getString(), false) -
                            tuning.getNote(new_string, false);
    const int new_fret = note.getFretNumber() + fret_offset;

    note.setFretNumber(new_fret);
    note.setString(new_string);

    // TODO - clear hammerons etc from adjacent position. However,
    // we can probably maintain most hammerons when a range of
    // notes is shifted.
}

void
ShiftString::redo()
{
    if (!myLocation.hasSelection())
    {
        // Single selected note.
        Position *position = myLocation.getPosition();
        Note *note = myLocation.getNote();

        // Record the original state of this position.
        myOriginalPositions.push_back(*position);

        const Tuning *tuning = findActiveTuning(myLocation);
        if (!tuning)
            return;

        shiftNote(*note, *tuning, myShiftUp);
    }
    else
    {
        // All notes in the range of selected positions.
        for (Position *position : myLocation.getSelectedPositions())
        {
            // Record the original state of this position.
            myOriginalPositions.push_back(*position);

            ScoreLocation current_location(myLocation);
            current_location.setPositionIndex(ScoreUtils::findIndexByPosition(
                myLocation.getVoice().getPositions(), position->getPosition()));

            const Tuning *tuning = findActiveTuning(current_location);
            if (!tuning)
                continue;

            for (Note &note : position->getNotes())
                shiftNote(note, *tuning, myShiftUp);
        }
    }
}

void
ShiftString::undo()
{
    assert(myLocation.getSelectedPositions().size() ==
           myOriginalPositions.size());

    std::vector<Position *> positions = myLocation.getSelectedPositions();
    for (size_t i = 0, n = positions.size(); i < n; ++i)
        *positions[i] = myOriginalPositions[i];
}
