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

#ifndef ACTIONS_CHORDDIAGRAM_H
#define ACTIONS_CHORDDIAGRAM_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/chorddiagram.h>

class Score;

class AddChordDiagram : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddChordDiagram)

public:
    AddChordDiagram(Score &score, const ChordDiagram &diagram, int index = -1);

    void redo() final;
    void undo() final;

private:
    Score &myScore;
    ChordDiagram myDiagram;
    int myDiagramIndex;
};

class RemoveChordDiagram : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveChordDiagram)

public:
    RemoveChordDiagram(Score &score, int idx);

    void redo() final;
    void undo() final;

private:
    Score &myScore;
    ChordDiagram myDiagram;
    int myDiagramIndex;
};

#endif

