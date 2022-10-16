/*
 * Copyright (C) 2022 Cameron White
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

#include "chorddiagram.h"

#include <score/score.h>

AddChordDiagram::AddChordDiagram(Score &score, const ChordDiagram &diagram,
                                 int index)
    : QUndoCommand(tr("Add Chord Diagram")),
      myScore(score),
      myDiagram(diagram),
      myDiagramIndex(index >= 0 ? index : static_cast<int>(score.getChordDiagrams().size()))
{
}

void
AddChordDiagram::redo()
{
    myScore.insertChordDiagram(myDiagram, myDiagramIndex);
}

void
AddChordDiagram::undo()
{
    myScore.removeChordDiagram(myDiagramIndex);
}

RemoveChordDiagram::RemoveChordDiagram(Score &score, int idx)
    : QUndoCommand(tr("Remove Chord Diagram")),
      myScore(score),
      myDiagram(score.getChordDiagrams()[idx]),
      myDiagramIndex(idx)
{
}

void
RemoveChordDiagram::redo()
{
    myScore.removeChordDiagram(myDiagramIndex);
}

void
RemoveChordDiagram::undo()
{
    myScore.insertChordDiagram(myDiagram, myDiagramIndex);
}

