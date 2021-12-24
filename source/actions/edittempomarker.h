/*
  * Copyright (C) 2012 Cameron White
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

#ifndef ACTIONS_EDITTEMPOMARKER_H
#define ACTIONS_EDITTEMPOMARKER_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/scorelocation.h>
#include <score/tempomarker.h>

class AddTempoMarker : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddTempoMarker)

public:
    AddTempoMarker(const ScoreLocation &location, const TempoMarker &marker);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const TempoMarker myMarker;
};

class RemoveTempoMarker : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveTempoMarker)

public:
    RemoveTempoMarker(const ScoreLocation &location);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const TempoMarker myOriginalTempo;
};

#endif
