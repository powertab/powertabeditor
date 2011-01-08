#include "staffdata.h"

StaffData::StaffData()
{
    height = 0;
    topEdge = 0;
    leftEdge = 0;
    width = 0;
    tabLineSpacing = 0;
    numOfStrings = 0;
    stdNotationStaffAboveSpacing = 0;
    stdNotationStaffBelowSpacing = 0;
    symbolSpacing = 0;
    positionWidth = 0;
    tabStaffBelowSpacing = 0;
}

// Calculates the offset of the tab staff from the top of the staff
int StaffData::getTabStaffOffset() const
{
    return height - tabStaffBelowSpacing - getTabStaffSize();
}
