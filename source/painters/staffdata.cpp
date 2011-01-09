#include "staffdata.h"

#include "../powertabdocument/staff.h"

const int StaffData::staffBorderSpacing = Staff::STAFF_BORDER_SPACING; // extra space around top and bottom of staves

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
    return height - tabStaffBelowSpacing - staffBorderSpacing - getTabStaffSize();
}

// Updates the height of the staff
void StaffData::calculateHeight()
{
    height = stdNotationStaffAboveSpacing + stdNotationStaffBelowSpacing + tabStaffBelowSpacing + symbolSpacing +
             getTabStaffSize() + getStdNotationStaffSize() + 4 * staffBorderSpacing;
}

// Calculates the size of the standard notation staff
int StaffData::getStdNotationStaffSize() const
{
    return (numOfStdNotationLines - 1) * stdNotationLineSpacing;
}

// Calculates the size of the tab staff
int StaffData::getTabStaffSize() const
{
    return (numOfStrings - 1) * tabLineSpacing;
}

// Calculates the height offset for the given tab line
// Optionally can calculate absolute/relative position
int StaffData::getTabLineHeight(int stringNum, bool absolutePos) const
{
    int offset = height - tabStaffBelowSpacing - staffBorderSpacing - (numOfStrings - stringNum) * tabLineSpacing;

    if (absolutePos)
    {
        offset += topEdge;
    }

    return offset;
}

// Calculates the height offset of the top line of the tab staff
int StaffData::getTopTabLine(bool absolutePos) const
{
    return getTabLineHeight(1, absolutePos);
}

// Calculates the height offset of the bottom line of the tab staff
int StaffData::getBottomTabLine(bool absolutePos) const
{
    return getTabLineHeight(numOfStrings, absolutePos);
}

// Calculates the height offset for the given standard notation staff line
// Optionally can calculate absolute/relative position
int StaffData::getStdNotationLineHeight(int lineNumber, bool absolutePos) const
{
    int offset = stdNotationStaffAboveSpacing + staffBorderSpacing + (lineNumber - 1) * stdNotationLineSpacing;

    if (absolutePos)
    {
        offset += topEdge;
    }

    return offset;
}

// Calculates the height offset for the given standard notation staff space
// Optionally can calculate absolute/relative position
double StaffData::getStdNotationSpaceHeight(int spaceNumber, bool absolutePos) const
{
    return (getStdNotationLineHeight(spaceNumber, absolutePos) + getStdNotationLineHeight(spaceNumber + 1, absolutePos)) / 2;
}

// Calculates the height offset of the top line of the standard notation staff
int StaffData::getTopStdNotationLine(bool absolutePos) const
{
    return getStdNotationLineHeight(1, absolutePos);
}

// Calculates the height offset of the bottom line of the standard notation staff
int StaffData::getBottomStdNotationLine(bool absolutePos) const
{
    return getStdNotationLineHeight(numOfStdNotationLines, absolutePos);
}
