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

#ifndef ACTIONS_SHIFTTABNUMBER_H
#define ACTIONS_SHIFTTABNUMBER_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <optional>
#include <score/position.h>
#include <score/scorelocation.h>
#include <vector>

/// Shift tab numbers to an adjacent string.
class ShiftString : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(ShiftString)

public:
    ShiftString(const ScoreLocation &location, bool shift_up);

    void undo() final;
    void redo() final;

private:
    ScoreLocation myLocation;
    const bool myShiftUp;

    std::vector<Position> myOriginalPositions;
    std::optional<Position> myOriginalPrevPosition;
};

#endif
