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
