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

#include <algorithm>
#include <boost/make_shared.hpp>

#include <powertabdocument/barline.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>
#include <powertabdocument/layout.h>

InsertNotes::InsertNotes(boost::shared_ptr<System> system, boost::shared_ptr<Staff> staff,
                         uint32_t insertionPos, const std::vector<Position*>& positions) :
    system(system), insertionPos(insertionPos),
    staffIndex(system->FindStaffIndex(staff)), newPositions(positions),
    originalSystem(boost::make_shared<System>(*system))
{
    setText(QObject::tr("Insert Notes"));
}

InsertNotes::~InsertNotes()
{
    qDeleteAll(newPositions);
}

void InsertNotes::redo()
{
    // adjust the locations of the new notes
    const int offset = insertionPos - newPositions.front()->GetPosition();
    for (size_t i = 0; i < newPositions.size(); i++)
    {
        newPositions[i]->SetPosition(newPositions[i]->GetPosition() + offset);
    }

    const size_t startPos = newPositions.front()->GetPosition();
    const size_t endPos = newPositions.back()->GetPosition();

    // Check for any existing notes or barlines that will conflict with the
    // new notes.
    boost::shared_ptr<Staff> staff = system->GetStaff(staffIndex);
    std::vector<Position*> currentPositions;
    std::vector<System::BarlinePtr> currentBarlines;
    staff->GetPositionsInRange(currentPositions, 0, startPos, endPos);
    system->GetBarlinesInRange(currentBarlines, startPos, endPos);

    // if there is a conflict, shift the old notes/barlines to the right.
    if (!currentPositions.empty() || !currentBarlines.empty())
    {
        const int firstPosition = currentPositions.empty() ? 0 :
                                        currentPositions.front()->GetPosition();
        const int firstBarPos = currentBarlines.empty() ? 0 :
                                        currentBarlines.front()->GetPosition();
        const int shiftAmount = newPositions.back()->GetPosition() -
                                std::max(firstPosition, firstBarPos) + 1;
        assert(shiftAmount > 0);

        for (int i = 0; i < shiftAmount; i++)
        {
            system->ShiftForward(insertionPos);
        }
    }

    // finally, insert the new notes
    for (size_t i = 0; i < newPositions.size(); i++)
    {
        staff->InsertPosition(0, newPositions[i]);
    }

    // Do a deep copy of the positions (they are owned by the staff they were
    // inserted into).
    std::transform(newPositions.begin(), newPositions.end(),
                   newPositions.begin(), std::mem_fun(&Position::CloneObject));
}

void InsertNotes::undo()
{
    // revert to the original system & staff
    *system = *originalSystem;
}
