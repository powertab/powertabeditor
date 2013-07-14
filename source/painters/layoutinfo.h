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
#include <vector>

class Barline;
class KeySignature;
class Position;
class Staff;
class System;
class TimeSignature;

class SymbolGroup
{
public:
    enum SymbolType
    {
        NoSymbol,
        PickStrokeUp,
        PickStrokeDown,
        Tap,
        Hammeron,
        Pulloff,
        Octave8va,
        Octave15ma
    };

    SymbolGroup(const Position &position, SymbolType symbol,
                double x, double width, double height);

    SymbolType getSymbolType() const;
    double getX() const;
    double getWidth() const;
    int getHeight() const;

private:
    /// First position associated with the symbol group.
    const Position *myPosition;
    /// The type of symbol.
    SymbolType mySymbolType;
    /// x-coordinate of the start of the symbol group.
    double myX;
    /// Width of the symbol group.
    double myWidth;
    /// Offset from the staff.
    int myHeight;
};

struct LayoutInfo
{
    LayoutInfo(const System& system, const Staff &staff);

    int getStringCount() const;

    double getStaffHeight() const;

    double getStdNotationLine(int line) const;
    double getStdNotationSpace(int space) const;
    double getTopStdNotationLine() const;
    double getBottomStdNotationLine() const;
    double getStdNotationStaffHeight() const;

    double getTabLine(int line) const;
    double getTopTabLine() const;
    double getBottomTabLine() const;
    double getTabLineSpacing() const;

    double getPositionSpacing() const;
    double getFirstPositionX() const;
    double getPositionX(int position) const;

    static const double STAFF_WIDTH;
    static const int NUM_STD_NOTATION_LINES;
    static const double STD_NOTATION_LINE_SPACING;
    /// Padding around top and bottom of staves.
    static const double STAFF_BORDER_SPACING;
    /// Padding surrounding a clef.
    static const double CLEF_PADDING;
    /// Space given to an accidental in a key signature;
    static const double ACCIDENTAL_WIDTH;
    /// Space given to the treble/bass clef.
    static const double CLEF_WIDTH;
    /// Space given to a system-level symbol (e.g. a rehearsal sign).
    static const double SYSTEM_SYMBOL_SPACING;
    /// Space given to a tab symbol (e.g. pickstroke).
    static const double TAB_SYMBOL_SPACING;

    static double centerItem(double xmin, double xmax, double width)
    {
        return (xmin + ((xmax - (xmin + width)) / 2) + 1);
    }

    static double getWidth(const KeySignature &key);
    static double getWidth(const TimeSignature &time);
    static double getWidth(const Barline &bar);

    const std::vector<SymbolGroup> &getTabStaffBelowSymbols() const;
    const std::vector<SymbolGroup> &getStdNotationStaffAboveSymbols() const;

private:
    static const double MIN_POSITION_SPACING;

    /// Gets the total width used by all key and time signatures that reside
    /// within the system (does not include the start bar). If the position
    /// is -1, traverse all barlines.
    double getCumulativeBarlineWidths(int position = -1) const;

    /// Compute an optimal position spacing for the system.
    void computePositionSpacing();

    /// Compute the spacing and layout of symbols that are drawn below the
    /// tab staff.
    void calculateTabStaffBelowLayout();

    /// Calculate the layout of symbols displayed above the standard notation
    /// staff. This is just the 8va and 15ma symbols.
    void calculateTabStaffAboveLayout();

    /// Returns the largest height of any symbol group.
    static int getMaxHeight(const std::vector<SymbolGroup> &groups);

    const System &mySystem;
    const Staff &myStaff;
    double myPositionSpacing;

    std::vector<SymbolGroup> myTabStaffBelowSymbols;
    double myTabStaffBelowSpacing;
    std::vector<SymbolGroup> myStdNotationStaffAboveSymbols;
    double myStdNotationStaffAboveSpacing;
};

typedef boost::shared_ptr<LayoutInfo> LayoutPtr;
typedef boost::shared_ptr<const LayoutInfo> LayoutConstPtr;

#endif // STAFFDATA_H
