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
  
#ifndef PAINTERS_LAYOUTINFO_H
#define PAINTERS_LAYOUTINFO_H

#include <boost/shared_ptr.hpp>

class Staff;

struct LayoutInfo
{
    LayoutInfo(const Staff &staff);

    int getStringCount() const;

    double getStaffHeight() const;
    double getStdNotationLine(int line) const;
    double getTopStdNotationLine() const;
    double getTabLine(int line) const;
    double getTopTabLine() const;
    double getTabLineSpacing() const;

    static const double STAFF_WIDTH;
    static const int NUM_STD_NOTATION_LINES;
    static const double STD_NOTATION_LINE_SPACING;
    /// Padding around top and bottom of staves.
    static const double STAFF_BORDER_SPACING;

private:
    const Staff &myStaff;
};

typedef boost::shared_ptr<LayoutInfo> LayoutPtr;
typedef boost::shared_ptr<const LayoutInfo> LayoutConstPtr;

#endif // STAFFDATA_H
