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
  
#ifndef ACTIONS_EDITTIMESIGNATURE_H
#define ACTIONS_EDITTIMESIGNATURE_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/scorelocation.h>
#include <score/timesignature.h>

class EditTimeSignature : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(EditTimeSignature)

public:
    EditTimeSignature(const ScoreLocation &location,
                      const TimeSignature &newTimeSig);

    virtual void redo() override;
    virtual void undo() override;

private:
    /// Updates all of the time signatures following myLocation until a
    /// different time signature is reached.
    void updateFollowingTimeSignatures(const TimeSignature &oldTime,
                                       const TimeSignature &newTime);

    ScoreLocation myLocation;
    const TimeSignature myNewTime;
    const TimeSignature myOldTime;
};

#endif
