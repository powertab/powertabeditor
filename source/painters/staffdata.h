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
  
#ifndef STAFFDATA_H
#define STAFFDATA_H

// Structure to hold data about a staff, and provide common layout-related calculations

class StaffData
{
public:
    int height;
    int topEdge;
    int leftEdge;
    int width;
    int tabLineSpacing;
    int numOfStrings;
    static const int numOfStdNotationLines = 5;
    int stdNotationStaffAboveSpacing;
    int stdNotationStaffBelowSpacing;
    int symbolSpacing;
    int positionWidth;
    int tabStaffBelowSpacing;
    static const int staffBorderSpacing;

    StaffData();

    int getTabLineHeight(int stringNum) const;
    int getTopTabLine() const;
    int getBottomTabLine() const;
    int getTabStaffSize() const;

    int getStdNotationLineHeight(int lineNumber) const;
    double getStdNotationSpaceHeight(int spaceNumber) const;
    int getTopStdNotationLine() const;
    int getBottomStdNotationLine() const;

    int getStdNotationStaffSize() const;

    double getNoteHeadRightEdge() const;

    void calculateHeight();

    int getTabStaffOffset() const;
};

#endif // STAFFDATA_H
