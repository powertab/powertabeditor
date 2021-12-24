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
  
#include "edittimesignature.h"

#include <score/barline.h>
#include <score/score.h>

EditTimeSignature::EditTimeSignature(const ScoreLocation &location,
                                     const TimeSignature& newTimeSig)
    : QUndoCommand(tr("Edit Time Signature")),
      myLocation(location),
      myNewTime(newTimeSig),
      myOldTime(location.getBarline()->getTimeSignature())
{
}

void EditTimeSignature::redo()
{
    myLocation.getBarline()->setTimeSignature(myNewTime);
    updateFollowingTimeSignatures(myOldTime, myNewTime);
}

void EditTimeSignature::undo()
{
    myLocation.getBarline()->setTimeSignature(myOldTime);
    updateFollowingTimeSignatures(myNewTime, myOldTime);
}

void EditTimeSignature::updateFollowingTimeSignatures(
        const TimeSignature &oldTime, const TimeSignature &newTime)
{
    Score &score = myLocation.getScore();
    const size_t start_system_index = myLocation.getSystemIndex();

    for (size_t i = start_system_index; i < score.getSystems().size(); ++i)
    {
        for (Barline &bar : score.getSystems()[i].getBarlines())
        {
            if (i == start_system_index &&
                bar.getPosition() <= myLocation.getPositionIndex())
            {
                continue;
            }

            const TimeSignature &currentTime = bar.getTimeSignature();
            if (currentTime.getMeterType() == oldTime.getMeterType() &&
                currentTime.getBeatsPerMeasure() == oldTime.getBeatsPerMeasure() &&
                currentTime.getBeatValue() == oldTime.getBeatValue())
            {
                TimeSignature time(newTime);
                time.setVisible(currentTime.isVisible());
                bar.setTimeSignature(time);
            }
            else
                return;
        }
    }
}
