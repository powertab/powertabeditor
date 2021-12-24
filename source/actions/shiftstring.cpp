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
#include <score/voiceutils.h>

ShiftString::ShiftString(const ScoreLocation &location, bool shift_up)
    : QUndoCommand(shift_up ? tr("Shift String Up")
                            : tr("Shift String Down")),
      myLocation(location),
      myShiftUp(shift_up)
{
}

namespace
{
struct ShiftItem
{
    ShiftItem(Voice &voice, const Tuning &tuning, const Position &position, Note &note);

    bool shift(bool shift_up, bool selection_start, bool selection_end);
    int getPosition() const { return myPosition.get().getPosition(); }

    // Using std::reference_wrapper to make the struct movable.
    std::reference_wrapper<Voice> myVoice;
    std::reference_wrapper<const Tuning> myTuning;
    std::reference_wrapper<const Position> myPosition;
    std::reference_wrapper<Note> myNote;
};

ShiftItem::ShiftItem(Voice &voice, const Tuning &tuning,
                     const Position &position, Note &note)
    : myVoice(voice), myTuning(tuning), myPosition(position), myNote(note)
{
}

bool
ShiftItem::shift(bool shift_up, bool selection_start, bool selection_end)
{
    auto clearNoteProperties = [](Note &note) {
        note.setProperty(Note::Tied, false);
        note.setProperty(Note::HammerOnOrPullOff, false);
        note.setProperty(Note::ShiftSlide, false);
        note.setProperty(Note::LegatoSlide, false);
    };

    Note &note = myNote;
    const Tuning &tuning = myTuning;

    // If this note is in the last position of the selection, remove any
    // properties that connect it to the next note (which isn't being shifted
    // along with it).
    if (selection_end)
        clearNoteProperties(note);

    // Similarly, at the start of the selection, clear any properties linked
    // from the previous note on the same string.
    if (selection_start)
    {
        // TODO - handle the case where the previous note is in the previous
        // system.
        Note *prev_note = VoiceUtils::getPreviousNote(
            myVoice, getPosition(), note.getString(), nullptr);
        if (prev_note)
            clearNoteProperties(*prev_note);
    }

    // Determine the new string and fret.
    const int new_string = note.getString() + (shift_up ? -1 : 1);
    if (new_string < 0 || new_string >= tuning.getStringCount())
        return false;

    // Can't create a conflict with another note.
    if (Utils::findByString(myPosition, new_string))
        return false;

    const int fret_offset = tuning.getNote(note.getString(), false) -
                            tuning.getNote(new_string, false);
    const int new_fret = note.getFretNumber() + fret_offset;
    if (new_fret < Note::MIN_FRET_NUMBER || new_fret > Note::MAX_FRET_NUMBER)
        return false;

    note.setFretNumber(new_fret);
    note.setString(new_string);

    return true;
}

const Tuning *
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

bool
shiftItems(std::vector<ShiftItem> &items, bool shift_up)
{
    const int first_position = items.front().getPosition();
    const int last_position = items.back().getPosition();

    // The notes within a position are ordered by string, so we want to process
    // in reverse order when shifting down so that notes on adjacent strings
    // can be shifted down one after the other.
    if (!shift_up)
        std::reverse(items.begin(), items.end());

    for (ShiftItem &item : items)
    {
        const bool selection_start = (item.getPosition() == first_position);
        const bool selection_end = (item.getPosition() == last_position);

        if (!item.shift(shift_up, selection_start, selection_end))
            return false;
    }

    return true;
}
} // namespace

void
ShiftString::redo()
{
    std::vector<ShiftItem> items;
    Voice &voice = myLocation.getVoice();

    // If there is a preceding position, we need to remove hammerons, etc
    // connected to the note being shifted.
    const Position *prev_pos = VoiceUtils::getPreviousPosition(
        voice, myLocation.getSelectedPositions().front()->getPosition());
    if (prev_pos)
        myOriginalPrevPosition = *prev_pos;

    if (!myLocation.hasSelection())
    {
        // Single selected note.
        Position *position = myLocation.getPosition();
        assert(position != nullptr);
        Note *note = myLocation.getNote();
        assert(note != nullptr);
        const Tuning *tuning = findActiveTuning(myLocation);

        // Record the original state of this position.
        myOriginalPositions.push_back(*position);

        if (tuning)
            items.emplace_back(voice, *tuning, *position, *note);
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
                voice.getPositions(), position->getPosition()));

            const Tuning *tuning = findActiveTuning(current_location);
            if (!tuning)
                continue;

            for (Note &note : position->getNotes())
                items.emplace_back(voice, *tuning, *position, note);
        }
    }

    // If some notes couldn't be shifted, revert.
    if (!shiftItems(items, myShiftUp))
        undo();
}

void
ShiftString::undo()
{
    assert(myLocation.getSelectedPositions().size() ==
           myOriginalPositions.size());

    std::vector<Position *> positions = myLocation.getSelectedPositions();
    for (size_t i = 0, n = positions.size(); i < n; ++i)
        *positions[i] = myOriginalPositions[i];

    myOriginalPositions.clear();

    if (myOriginalPrevPosition)
    {
        Position *prev_pos = VoiceUtils::getPreviousPosition(
            myLocation.getVoice(),
            myLocation.getSelectedPositions().front()->getPosition());
        assert(prev_pos);
        *prev_pos = *myOriginalPrevPosition;
        myOriginalPrevPosition.reset();
    }
}
