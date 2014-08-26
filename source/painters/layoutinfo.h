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

#include <array>
#include <memory>
#include <painters/beamgroup.h>
#include <painters/stdnotationnote.h>
#include <score/staff.h>
#include <vector>

class Barline;
class KeySignature;
class Score;
class System;
class TimeSignature;
class VerticalLayout;

class SymbolGroup
{
public:
    enum SymbolType
    {
        NoSymbol,

        // Below tab staff.
        PickStrokeUp,
        PickStrokeDown,
        Tap,
        Hammeron,
        Pulloff,
        Slide,

        // Above standard notation staff.
        Octave8va,
        Octave15ma,

        // Below standard notation staff.
        Octave8vb,
        Octave15mb,

        // Above tab staff.
        Bend,
        LetRing,
        Vibrato,
        WideVibrato,
        PalmMuting,
        TremoloPicking,
        Trill,
        NaturalHarmonic,
        Dynamic,
        ArtificialHarmonic // Note that this also appears below the tab staff.
    };

    SymbolGroup(SymbolType symbol, int leftPosition, int rightPosition,
                const Voice &voice, double width, int height);

    SymbolType getSymbolType() const;
    int getLeftPosition() const;
    int getRightPosition() const;
    const Voice &getVoice() const;
    double getWidth() const;
    int getHeight() const;

private:
    /// The type of symbol.
    SymbolType mySymbolType;
    /// First position of the symbol group.
    int myLeftPosition;
    /// Last position of the symbol group.
    int myRightPosition;
    /// Source voice of the symbol.
    const Voice *myVoice;
    /// Width of the symbol group.
    double myWidth;
    /// Offset from the staff.
    int myHeight;
};

struct LayoutInfo
{
    LayoutInfo(const Score &score, const System& system, int systemIndex,
               const Staff &staff, int staffIndex);

    int getStringCount() const;

    double getSystemSymbolSpacing() const;
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
    double getTabStaffHeight() const;

    double getPositionSpacing() const;
    int getNumPositions() const;
    double getFirstPositionX() const;
    double getPositionX(int position) const;
    int getPositionFromX(double x) const;

    static const double STAFF_WIDTH;
    static const int NUM_STD_NOTATION_LINES;
    static const double STD_NOTATION_LINE_SPACING;
    /// Padding around top and bottom of staves.
    static const double STAFF_BORDER_SPACING;
    /// Padding surrounding a clef.
    static const double CLEF_PADDING;
    /// Padding between the bar number and the system.
    static const double BAR_NUMBER_PADDING;
    /// Space given to an accidental in a key signature;
    static const double ACCIDENTAL_WIDTH;
    /// Space given to the treble/bass clef.
    static const double CLEF_WIDTH;
    /// Space given to a system-level symbol (e.g. a rehearsal sign).
    static const double SYSTEM_SYMBOL_SPACING;
    /// Space given to a tab symbol (e.g. pickstroke).
    static const double TAB_SYMBOL_SPACING;
    /// Default position spacing.
    static const double DEFAULT_POSITION_SPACING;
    /// Distance between the note stem and the horizontal line of an irregular
    /// group.
    static const double IRREGULAR_GROUP_HEIGHT;
    /// Distance between the note stem and the vertical lines of an irregular
    /// group.
    static const double IRREGULAR_GROUP_BEAM_SPACING;

    static double centerItem(double xmin, double xmax, double width)
    {
        return (xmin + ((xmax - (xmin + width)) / 2) + 1);
    }

    static double getWidth(const KeySignature &key);
    static double getWidth(const TimeSignature &time);
    static double getWidth(const Barline &bar);

    double getTabStaffBelowSpacing() const;
    const std::vector<SymbolGroup> &getTabStaffBelowSymbols() const;
    const std::vector<SymbolGroup> &getTabStaffAboveSymbols() const;
    const std::vector<SymbolGroup> &getStdNotationStaffAboveSymbols() const;

    double getStdNotationStaffBelowSpacing() const;
    const std::vector<SymbolGroup> &getStdNotationStaffBelowSymbols() const;

    const std::vector<StdNotationNote> &getStdNotationNotes() const;
    const std::vector<BeamGroup> &getBeamGroups(int voice) const;
    const std::vector<NoteStem> &getNoteStems(int voice) const;

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
    void calculateStdNotationStaffAboveLayout();

    /// Calculate the layout of symbols displayed below the standard notation
    /// staff. This is just the 8vb and 15mb symbols.
    void calculateStdNotationStaffBelowLayout();

    /// Helper function for calculating the spacing above/below the standard
    /// notation staff.
    void calculateOctaveSymbolLayout(std::vector<SymbolGroup> &symbols,
                                     bool aboveStaff);

    /// Calculate the layout of symbols between the standard notation and
    /// tab staves.
    void calculateTabStaffAboveLayout();

    /// Computes the layout for bends in the staff.
    void calculateBendLayout(VerticalLayout &layout);

    /// Returns the largest height of any symbol group.
    static int getMaxHeight(const std::vector<SymbolGroup> &groups);

    const System &mySystem;
    const Staff &myStaff;
    int myLineSpacing;
    double myPositionSpacing;
    int myNumPositions;

    std::vector<SymbolGroup> myTabStaffBelowSymbols;
    double myTabStaffBelowSpacing;
    std::vector<SymbolGroup> myTabStaffAboveSymbols;
    double myTabStaffAboveSpacing;
    std::vector<SymbolGroup> myStdNotationStaffAboveSymbols;
    double myStdNotationStaffAboveSpacing;
    std::vector<SymbolGroup> myStdNotationStaffBelowSymbols;
    double myStdNotationStaffBelowSpacing;

    std::vector<StdNotationNote> myNotes;
    // Build a separate list of stems and beam groups for each voice.
    std::array<std::vector<BeamGroup>, Staff::NUM_VOICES> myBeamGroups;
    std::array<std::vector<NoteStem>, Staff::NUM_VOICES> myStems;
};

typedef std::shared_ptr<LayoutInfo> LayoutPtr;
typedef std::shared_ptr<const LayoutInfo> LayoutConstPtr;

#endif
