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

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <boost/foreach.hpp>

EditTimeSignature::EditTimeSignature(Score* score, const SystemLocation& location,
                                     const TimeSignature& newTimeSig) :
    QUndoCommand(QObject::tr("Edit Time Signature")),
    score(score),
    location(location),
    newTimeSig(newTimeSig)
{
    // save the original time signature
    Score::SystemConstPtr system = score->GetSystem(location.getSystemIndex());
    System::BarlineConstPtr barline = system->GetBarlineAtPosition(location.getPositionIndex());
    Q_ASSERT(barline);
    oldTimeSig = barline->GetTimeSignature();
}

void EditTimeSignature::redo()
{
    switchTimeSignatures(oldTimeSig, newTimeSig);
}

void EditTimeSignature::undo()
{
    switchTimeSignatures(newTimeSig, oldTimeSig);
}

/// Switches from the old time signature to the new time signature, starting at the position
/// stored in the "location" member
/// Modifies all following time signatures until a different time signature is reached
void EditTimeSignature::switchTimeSignatures(const TimeSignature& oldTime,
                                             const TimeSignature& newTime)
{
    const size_t startSystem = location.getSystemIndex();
    for (size_t i = startSystem; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr system = score->GetSystem(i);
        std::vector<System::BarlinePtr> barlines;
        system->GetBarlines(barlines);

        BOOST_FOREACH(System::BarlinePtr barline, barlines)
        {
            if (i == startSystem && barline->GetPosition() < location.getPositionIndex())
            {
                continue;
            }

            TimeSignature& time = barline->GetTimeSignature();
            if (time.IsSameMeter(oldTime))
            {
                // modify everything except for visibility
                const bool isShown = time.IsShown();
                time = newTime;
                time.SetShown(isShown);

                // only modify visibility for the time signature that was edited directly
                if (i == startSystem && barline->GetPosition() == location.getPositionIndex())
                {
                    time.SetShown(newTime.IsShown());
                }
            }
            else
            {
                system->AdjustPositionSpacing();
                return;
            }
        }

        system->AdjustPositionSpacing();
    }
}
