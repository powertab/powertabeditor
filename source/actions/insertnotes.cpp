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

InsertNotes::InsertNotes(const ScoreLocation &location,
                         const std::vector<Position> &positions,
                         const std::vector<IrregularGrouping> &groups)
    : QUndoCommand(tr("Insert Notes")),
      myLocation(location),
      myNewPositions(positions),
      myNewGroups(groups),
      myShiftAmount(0)
{
    const int insertionPos = location.getPositionIndex();

    // Adjust the locations of the new items.
    const int offset = insertionPos - myNewPositions.front().getPosition();
    for (Position &pos: myNewPositions)
        pos.setPosition(pos.getPosition() + offset);
    for (IrregularGrouping &group : myNewGroups)
        group.setPosition(group.getPosition() + offset);

    const int startPos = myNewPositions.front().getPosition();
    const int endPos = myNewPositions.back().getPosition();

    const System &system = location.getSystem();
    const Position *nextPos =
        VoiceUtils::getNextPosition(location.getVoice(), startPos - 1);
    const Barline *nextBar =
        system.getNextBarline(startPos == 0 ? startPos : startPos - 1);

    // Check for any existing notes or barlines that will conflict with the
    // new notes.
    std::optional<int> position;
    if (nextPos && nextPos->getPosition() <= endPos)
        position = nextPos->getPosition();
    if (nextBar && nextBar->getPosition() <= endPos)
    {
        if (position)
            position = std::min(*position, nextBar->getPosition());
        else
            position = nextBar->getPosition();
    }

    if (position)
    {
        myShiftAmount = myNewPositions.back().getPosition() - *position + 1;
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
}

void InsertNotes::undo()
{
    // Remove the items that were added.
    for (const Position &pos : myNewPositions)
        myLocation.getVoice().removePosition(pos);

    for (const IrregularGrouping &group : myNewGroups)
        myLocation.getVoice().removeIrregularGrouping(group);

    // Undo any shifting that was performed.
    for (int i = 0; i < myShiftAmount; ++i)
    {
        SystemUtils::shiftBackward(myLocation.getSystem(),
                                   myLocation.getPositionIndex());
    }
}
