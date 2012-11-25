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
  
#include "edittuning.h"

#include <powertabdocument/guitar.h>
#include <powertabdocument/note.h>
#include <powertabdocument/position.h>
#include <powertabdocument/score.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>
#include <widgets/mixer/mixerinstrument.h>

EditTuning::EditTuning(MixerInstrument* mixer, Score* score,
                       boost::shared_ptr<Guitar> guitar,
                       const Tuning& newTuning) :
    mixer(mixer),
    score(score),
    guitar(guitar),
    newTuning(newTuning),
    oldTuning(guitar->GetTuning())
{
    setText(QObject::tr("Edit Tuning"));
}

void EditTuning::redo()
{
    score->SetTuning(guitar, newTuning);
    mixer->update();
}

void EditTuning::undo()
{
    score->SetTuning(guitar, oldTuning);
    mixer->update();
}

/// Determines whether the new tuning is safe to use (e.g. there aren't notes on
/// the strings that would be deleted). Checks all of the notes played by the
/// guitar.
bool EditTuning::canChangeTuning(const Score* score,
                                 boost::shared_ptr<Guitar> guitar,
                                 const Tuning& newTuning)
{
    if (guitar->GetTuning().GetStringCount() <= newTuning.GetStringCount())
    {
        return true;
    }

    const uint32_t staffIndex = guitar->GetNumber();
    const size_t numStrings = newTuning.GetStringCount();

    for (size_t i = 0; i < score->GetSystemCount(); ++i)
    {
        System::StaffConstPtr staff = score->GetSystem(i)->GetStaff(staffIndex);
        if (!staff->IsValidTablatureStaffType(numStrings))
        {
            return false;
        }
    }

    return true;
}
