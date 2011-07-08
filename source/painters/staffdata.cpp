#include "staffdata.h"

#include <powertabdocument/staff.h>

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

/// Calculates the offset of the tab staff from the top of the staff
int StaffData::getTabStaffOffset() const
{
    return height - tabStaffBelowSpacing - staffBorderSpacing - getTabStaffSize();
}

/// Updates the height of the staff
void StaffData::calculateHeight()
{
    height = stdNotationStaffAboveSpacing + stdNotationStaffBelowSpacing + tabStaffBelowSpacing + symbolSpacing +
             getTabStaffSize() + getStdNotationStaffSize() + 4 * staffBorderSpacing;
}

/// Calculates the size of the standard notation staff
int StaffData::getStdNotationStaffSize() const
{
    return (numOfStdNotationLines - 1) * Staff::STD_NOTATION_LINE_SPACING;
}

/// Calculates the size of the tab staff
int StaffData::getTabStaffSize() const
{
    return (numOfStrings - 1) * tabLineSpacing;
}

/// Calculates the height offset for the given tab line
int StaffData::getTabLineHeight(int stringNum) const
{
    int offset = height - tabStaffBelowSpacing - staffBorderSpacing - (numOfStrings - stringNum) * tabLineSpacing;

    return offset;
}

/// Calculates the height offset of the top line of the tab staff
int StaffData::getTopTabLine() const
{
    return getTabLineHeight(1);
}

/// Calculates the height offset of the bottom line of the tab staff
int StaffData::getBottomTabLine() const
{
    return getTabLineHeight(numOfStrings);
}

/// Calculates the height offset for the given standard notation staff line
int StaffData::getStdNotationLineHeight(int lineNumber) const
{
    int offset = stdNotationStaffAboveSpacing + staffBorderSpacing + (lineNumber - 1) * Staff::STD_NOTATION_LINE_SPACING;

    return offset;
}

/// Calculates the height offset for the given standard notation staff space
double StaffData::getStdNotationSpaceHeight(int spaceNumber) const
{
    return (getStdNotationLineHeight(spaceNumber) + getStdNotationLineHeight(spaceNumber + 1)) / 2;
}

/// Calculates the height offset of the top line of the standard notation staff
int StaffData::getTopStdNotationLine() const
{
    return getStdNotationLineHeight(1);
}

/// Calculates the height offset of the bottom line of the standard notation staff
int StaffData::getBottomStdNotationLine() const
{
    return getStdNotationLineHeight(numOfStdNotationLines);
}

/// Gives the position of the right edge of a note head, relative to the start of a position
double StaffData::getNoteHeadRightEdge() const
{
    return 0.75 * positionWidth;
}
