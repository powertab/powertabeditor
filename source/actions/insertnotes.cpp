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

#include <powertabdocument/barline.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>

InsertNotes::InsertNotes(boost::shared_ptr<System> system,
                         boost::shared_ptr<Staff> staff,
                         uint32_t insertionPos,
                         const std::vector<Position*>& positions) :
    QUndoCommand(QObject::tr("Insert Notes")),
    system(system),
    staff(staff),
    insertionPos(insertionPos),
    newPositions(positions),
    shiftAmount(0),
    ownPositions(true)
{
    // Adjust the locations of the new notes.
    const int offset = insertionPos - newPositions.front()->GetPosition();
    for (size_t i = 0; i < newPositions.size(); i++)
    {
        newPositions[i]->SetPosition(newPositions[i]->GetPosition() + offset);
    }

    const size_t startPos = newPositions.front()->GetPosition();
    const size_t endPos = newPositions.back()->GetPosition();

    // Check for any existing notes or barlines that will conflict with the
    // new notes.
    std::vector<Position*> currentPositions;
    std::vector<System::BarlinePtr> currentBarlines;
    staff->GetPositionsInRange(currentPositions, 0, startPos, endPos);
    system->GetBarlinesInRange(currentBarlines, startPos, endPos);

    // If there is a conflict, we will need toshift the old notes/barlines to
    // the right.
    if (!currentPositions.empty() || !currentBarlines.empty())
    {
        const int firstPosition = currentPositions.empty() ? -1 :
                                        currentPositions.front()->GetPosition();
        const int firstBarPos = currentBarlines.empty() ? -1 :
                                        currentBarlines.front()->GetPosition();
        if (firstPosition >= 0 || firstBarPos > 0)
        {
            shiftAmount = newPositions.back()->GetPosition() -
                    std::max(firstPosition, firstBarPos) + 1;
            assert(shiftAmount > 0);
        }
    }
}

InsertNotes::~InsertNotes()
{
    if (ownPositions)
    {
        qDeleteAll(newPositions);
    }
}

void InsertNotes::redo()
{
    // Shift existing notes / barlines to the right if necessary.
    for (int i = 0; i < shiftAmount; i++)
    {
        system->ShiftForward(insertionPos);
    }

    // Insert the new notes.
    for (size_t i = 0; i < newPositions.size(); i++)
    {
        staff->InsertPosition(0, newPositions[i]);
    }

    ownPositions = false;
}

void InsertNotes::undo()
{
    // Remove the notes that were added.
    for (size_t i = 0; i < newPositions.size(); i++)
    {
        staff->RemovePosition(0, newPositions[i]->GetPosition());
    }

    // Undo any shifting that was performed.
    for (int i = 0; i < shiftAmount; i++)
    {
        system->ShiftBackward(insertionPos);
    }

    ownPositions = true;
}
