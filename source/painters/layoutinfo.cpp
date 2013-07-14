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
  
#include "layoutinfo.h"

#include <score/staff.h>

const double LayoutInfo::STAFF_WIDTH = 750;
const int LayoutInfo::NUM_STD_NOTATION_LINES = 5;
const double LayoutInfo::STD_NOTATION_LINE_SPACING = 7;
const double LayoutInfo::STAFF_BORDER_SPACING = 10;
const double LayoutInfo::CLEF_PADDING = 3;

// TODO - compute these values based on the note positions, etc.
namespace {
const double STD_NOTATION_STAFF_ABOVE_SPACING = 9;
const double STD_NOTATION_STAFF_BELOW_SPACING = 9;
const double SYMBOL_SPACING = 0;
const double TAB_STAFF_BELOW_SPACING = 0;
}

LayoutInfo::LayoutInfo(const Staff &staff)
    : myStaff(staff)
{
}

int LayoutInfo::getStringCount() const
{
    return myStaff.getStringCount();
}

double LayoutInfo::getStaffHeight() const
{
    return STD_NOTATION_STAFF_ABOVE_SPACING + STD_NOTATION_STAFF_BELOW_SPACING +
            SYMBOL_SPACING + TAB_STAFF_BELOW_SPACING +
            STD_NOTATION_LINE_SPACING * (NUM_STD_NOTATION_LINES - 1) +
            (getStringCount() - 1) * getTabLineSpacing() +
            4 * STAFF_BORDER_SPACING;
}

double LayoutInfo::getStdNotationLine(int line) const
{
    return STD_NOTATION_STAFF_ABOVE_SPACING + STAFF_BORDER_SPACING +
            (line - 1) * STD_NOTATION_LINE_SPACING;
}

double LayoutInfo::getTopStdNotationLine() const
{
    return getStdNotationLine(1);
}

double LayoutInfo::getTabLine(int line) const
{
    return getStaffHeight() - TAB_STAFF_BELOW_SPACING - STAFF_BORDER_SPACING -
            (getStringCount() - line) * getTabLineSpacing();
}

double LayoutInfo::getTopTabLine() const
{
    return getTabLine(1);
}

double LayoutInfo::getTabLineSpacing() const
{
    return 9;
}
