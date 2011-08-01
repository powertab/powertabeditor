#include "insertnotes.h"

#include <algorithm>

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>
#include <powertabdocument/layout.h>

InsertNotes::InsertNotes(std::shared_ptr<System> system, std::shared_ptr<Staff> staff,
                         uint32_t insertionPos, const std::vector<Position*>& positions) :
    system(system), staff(staff),
    insertionPos(insertionPos), newPositions(positions),
    originalSystem(std::make_shared<System>(*system)),
    positionsInUse(false)
{
    setText(QObject::tr("Insert Notes"));
}

InsertNotes::~InsertNotes()
{
    if (!positionsInUse)
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

    // check for any existing notes that will conflict with the new notes
    std::vector<Position*> currentPositions;
    staff->GetPositionsInRange(currentPositions, 0, newPositions.front()->GetPosition(),
                               newPositions.back()->GetPosition());

    // if there is a conflict, shift the old notes to the right to make room
    if (!currentPositions.empty())
    {
        const int shiftAmount = newPositions.back()->GetPosition() -
                                currentPositions.front()->GetPosition() + 1;
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

    Layout::FormatSystem(system);
    positionsInUse = true;
}

void InsertNotes::undo()
{
    // do a deep copy of the positions (they are owned by the staff they were previously inserted into)
    std::transform(newPositions.begin(), newPositions.end(),
                   newPositions.begin(), std::mem_fun(&Position::CloneObject));
    positionsInUse = false;

    // revert to the original system & staff
    const uint32_t staffIndex = system->FindStaffIndex(staff);
    *system = *originalSystem;
    staff = system->GetStaff(staffIndex);
}
