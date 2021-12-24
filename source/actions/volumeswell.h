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

#ifndef ACTIONS_ADDVOLUMESWELL_H
#define ACTIONS_ADDVOLUMESWELL_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/position.h>
#include <score/scorelocation.h>

class AddVolumeSwell : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddVolumeSwell)

public:
    AddVolumeSwell(const ScoreLocation &location, const VolumeSwell &swell);

    void redo() final;
    void undo() final;

private:
    ScoreLocation myLocation;
    VolumeSwell mySwell;
};

class RemoveVolumeSwell : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveVolumeSwell)

public:
    RemoveVolumeSwell(const ScoreLocation &location);

    void redo() final;
    void undo() final;

private:
    ScoreLocation myLocation;
    VolumeSwell mySwell;
};

#endif
