/*
  * Copyright (C) 2011 Cameron White
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
  
#include "insertnotes.h"

#include <optional>
#include <score/system.h>
#include <score/voiceutils.h>

InsertNotes::InsertNotes(const ScoreLocation &location, const std::vector<Position> &positions,
                         const std::vector<IrregularGrouping> &groups,
                         const std::vector<Barline> &barlines)
    : QUndoCommand(tr("Insert Notes")),
      myLocation(location),
      myNewPositions(positions),
      myNewGroups(groups),
      myNewBarlines(barlines),
      myShiftAmount(0)
{
    const int src_first_barline_pos =
        !myNewBarlines.empty() ? myNewBarlines.front().getPosition() : INT_MAX;
    const int src_first_pos =
        !myNewPositions.empty() ? myNewPositions.front().getPosition() : INT_MAX;
    const int src_start_pos = std::min(src_first_barline_pos, src_first_pos);

    const int src_last_barline_pos =
        !myNewBarlines.empty() ? myNewBarlines.back().getPosition() : -1;
    const int src_last_pos = !myNewPositions.empty() ? myNewPositions.back().getPosition() : -1;
    const int src_end_pos = std::max(src_last_barline_pos, src_last_pos);

    int insertion_pos = location.getPositionIndex();
    // If we're pasting at the start of the system and our selection starts with a bar, shift
    // forward to avoid overwriting the existing start bar.
    if (insertion_pos == 0 && src_start_pos == src_first_barline_pos)
        ++insertion_pos;

    // Adjust the locations of the new items.
    const int offset = insertion_pos - src_start_pos;
    for (Position &pos: myNewPositions)
        pos.setPosition(pos.getPosition() + offset);
    for (IrregularGrouping &group : myNewGroups)
        group.setPosition(group.getPosition() + offset);
    for (Barline &barline : myNewBarlines)
        barline.setPosition(barline.getPosition() + offset);

    const int start_pos = src_start_pos + offset;
    const int end_pos = src_end_pos + offset;

    const System &system = location.getSystem();
    const Position *next_pos = VoiceUtils::getNextPosition(location.getVoice(), start_pos - 1);
    const Barline *next_bar = system.getNextBarline(start_pos == 0 ? start_pos : start_pos - 1);

    // Check for any existing notes or barlines that will conflict with the
    // new notes.
    std::optional<int> position;
    if (next_pos && next_pos->getPosition() <= end_pos)
        position = next_pos->getPosition();
    if (next_bar && next_bar->getPosition() <= end_pos)
        position = std::min(next_bar->getPosition(), position.value_or(INT_MAX));

    if (position)
    {
        myShiftAmount = end_pos - *position + 1;
        assert(myShiftAmount > 0);
    }
}

void InsertNotes::redo()
{
    // Shift existing notes / barlines to the right if necessary.
    for (int i = 0; i < myShiftAmount; ++i)
    {
        SystemUtils::shiftForward(myLocation.getSystem(),
                                  myLocation.getPositionIndex());
    }

    // Insert the new items.
    for (const Position &pos : myNewPositions)
        myLocation.getVoice().insertPosition(pos);

    for (const IrregularGrouping &group : myNewGroups)
        myLocation.getVoice().insertIrregularGrouping(group);

    for (const Barline &barline : myNewBarlines)
        myLocation.getSystem().insertBarline(barline);
}

void InsertNotes::undo()
{
    // Remove the items that were added.
    for (const Position &pos : myNewPositions)
        myLocation.getVoice().removePosition(pos);

    for (const IrregularGrouping &group : myNewGroups)
        myLocation.getVoice().removeIrregularGrouping(group);

    for (const Barline &barline : myNewBarlines)
        myLocation.getSystem().removeBarline(barline);

    // Undo any shifting that was performed.
    for (int i = 0; i < myShiftAmount; ++i)
    {
        SystemUtils::shiftBackward(myLocation.getSystem(),
                                   myLocation.getPositionIndex());
    }
}
