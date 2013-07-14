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

#include <boost/foreach.hpp>
#include <score/keysignature.h>
#include <score/staff.h>
#include <score/system.h>
#include <score/timesignature.h>

const double LayoutInfo::STAFF_WIDTH = 750;
const int LayoutInfo::NUM_STD_NOTATION_LINES = 5;
const double LayoutInfo::STD_NOTATION_LINE_SPACING = 7;
const double LayoutInfo::STAFF_BORDER_SPACING = 10;
const double LayoutInfo::CLEF_PADDING = 3;
const double LayoutInfo::ACCIDENTAL_WIDTH = 6;
const double LayoutInfo::CLEF_WIDTH = 22;
const double LayoutInfo::SYSTEM_SYMBOL_SPACING = 18;
const double LayoutInfo::MIN_POSITION_SPACING = 3;
const double LayoutInfo::TAB_SYMBOL_SPACING = 10;

// TODO - compute these values based on the note positions, etc.
namespace {
const double STD_NOTATION_STAFF_ABOVE_SPACING = 9;
const double STD_NOTATION_STAFF_BELOW_SPACING = 9;
const double SYMBOL_SPACING = 0;
}

LayoutInfo::LayoutInfo(const System &system, const Staff &staff)
    : mySystem(system),
      myStaff(staff),
      myPositionSpacing(20),
      myTabStaffBelowSpacing(0)
{
    computePositionSpacing();
    calculateTabStaffBelowLayout();
}

int LayoutInfo::getStringCount() const
{
    return myStaff.getStringCount();
}

double LayoutInfo::getStaffHeight() const
{
    return STD_NOTATION_STAFF_ABOVE_SPACING + STD_NOTATION_STAFF_BELOW_SPACING +
            SYMBOL_SPACING + myTabStaffBelowSpacing +
            STD_NOTATION_LINE_SPACING * (NUM_STD_NOTATION_LINES - 1) +
            (getStringCount() - 1) * getTabLineSpacing() +
            4 * STAFF_BORDER_SPACING;
}

double LayoutInfo::getStdNotationLine(int line) const
{
    return STD_NOTATION_STAFF_ABOVE_SPACING + STAFF_BORDER_SPACING +
            (line - 1) * STD_NOTATION_LINE_SPACING;
}

double LayoutInfo::getStdNotationSpace(int space) const
{
    return (getStdNotationLine(space) + getStdNotationLine(space + 1)) / 2.0;
}

double LayoutInfo::getTopStdNotationLine() const
{
    return getStdNotationLine(1);
}

double LayoutInfo::getBottomStdNotationLine() const
{
    return getStdNotationLine(NUM_STD_NOTATION_LINES);
}

double LayoutInfo::getStdNotationStaffHeight() const
{
    return (NUM_STD_NOTATION_LINES - 1) * STD_NOTATION_LINE_SPACING;
}

double LayoutInfo::getTabLine(int line) const
{
    return getStaffHeight() - myTabStaffBelowSpacing - STAFF_BORDER_SPACING -
            (getStringCount() - line) * getTabLineSpacing();
}

double LayoutInfo::getTopTabLine() const
{
    return getTabLine(1);
}

double LayoutInfo::getBottomTabLine() const
{
    return getTabLine(getStringCount());
}

double LayoutInfo::getTabLineSpacing() const
{
    return 9;
}

double LayoutInfo::getPositionSpacing() const
{
    return myPositionSpacing;
}

double LayoutInfo::getFirstPositionX() const
{
    double width = CLEF_WIDTH;
    const Barline &startBar = mySystem.getBarlines()[0];

    const double keyWidth = getWidth(startBar.getKeySignature());
    width += keyWidth;
    const double timeWidth = getWidth(startBar.getTimeSignature());
    width += timeWidth;

    // If we have both a key and time signature, they are separated by 3 units.
    if (keyWidth > 0 && timeWidth > 0)
        width += 3;

    // Add the width required by the starting barline; for a standard barline,
    // this is 1 unit of space, otherwise it is the distance between positions
    const double barlineWidth = (startBar.getBarType() == Barline::SingleBar)
            ? 1 : getPositionSpacing();
    width += barlineWidth;

    return width;
}

double LayoutInfo::getPositionX(int position) const
{
    double x = getFirstPositionX();
    // Include the width of all key/time signatures.
    x += getCumulativeBarlineWidths(position);
    // Move over 'n' positions.
    x += (position + 1) * getPositionSpacing();
    return x;
}

double LayoutInfo::getWidth(const KeySignature &key)
{
    double width = 0;

    if (key.isVisible())
        width = key.getNumAccidentals(true) * ACCIDENTAL_WIDTH;

    return width;
}

double LayoutInfo::getWidth(const TimeSignature &time)
{
    return time.isVisible() ? 18 : 0;
}

double LayoutInfo::getWidth(const Barline &bar)
{
    double width = 0;

    // Add the width of the key signature
    width += getWidth(bar.getKeySignature());

    // If the key signature has width, we need to adjust to account the right
    // side of the barline.
    if (width > 0)
    {
        // Some bars are thicker than others.
        if (bar.getBarType() == Barline::DoubleBar)
            width += 2;
        else if (bar.getBarType() == Barline::RepeatStart)
            width += 5;
        else if (bar.getBarType() == Barline::RepeatEnd ||
                 bar.getBarType() == Barline::DoubleBarFine)
            width += 6;
    }

    // Add the width of the time signature.
    double timeSignatureWidth = getWidth(bar.getTimeSignature());
    if (timeSignatureWidth > 0)
    {
        // 3 units of space from barline or key signature.
        width += 3 + timeSignatureWidth;
    }

    return width;
}

double LayoutInfo::getCumulativeBarlineWidths(int position) const
{
    double width = 0;

    const bool allBarlines = (position == -1);

    BOOST_FOREACH(const Barline &barline, mySystem.getBarlines())
    {
        if (barline == mySystem.getBarlines().front() ||
            barline == mySystem.getBarlines().back())
            continue;

        if (allBarlines || barline.getPosition() < position)
            width += getWidth(barline);
        else
            break;
    }

    return width;
}

void LayoutInfo::computePositionSpacing()
{
    const double width = getFirstPositionX() + getCumulativeBarlineWidths();

    // Find the number of positions needed for the system.
    int maxPos = 0;
    BOOST_FOREACH(const Staff &staff, mySystem.getStaves())
    {
        for (int i = 0; i < Staff::NUM_VOICES; ++i)
        {
            BOOST_FOREACH(const Position &position, staff.getVoice(i))
            {
                maxPos = std::max(maxPos, position.getPosition());
            }
        }
    }

    // TODO - include chord text, tempo markers, etc.
    BOOST_FOREACH(const Barline &barline, mySystem.getBarlines())
    {
        maxPos = std::max(maxPos, barline.getPosition());
    }

    const double availableSpace = STAFF_WIDTH - width;
    myPositionSpacing = availableSpace / (maxPos + 2);
}

void LayoutInfo::calculateTabStaffBelowLayout()
{
    for (int voice = 0; voice < Staff::NUM_VOICES; ++voice)
    {
        BOOST_FOREACH(const Position &pos, myStaff.getVoice(voice))
        {
            int height = 1;
            const int position = pos.getPosition();
            const double x = getPositionX(position);
            const double width = getPositionSpacing();

            if (pos.hasProperty(Position::PickStrokeUp))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(pos, SymbolGroup::PickStrokeUp, x,
                                        width, height++));
            }

            if (pos.hasProperty(Position::PickStrokeDown))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(pos, SymbolGroup::PickStrokeDown, x,
                                        width, height++));
            }

            if (pos.hasProperty(Position::Tap) ||
                Utils::hasNoteWithTappedHarmonic(pos))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(pos, SymbolGroup::Tap, x,
                                        width, height++));
            }

            if (Utils::hasNoteWithProperty(pos, Note::HammerOn) ||
                Utils::hasNoteWithProperty(pos, Note::HammerOnFromNowhere))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(pos, SymbolGroup::Hammeron, x,
                                        width, height++));
            }
            else if (Utils::hasNoteWithProperty(pos, Note::PullOff) ||
                     Utils::hasNoteWithProperty(pos, Note::PullOffToNowhere))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(pos, SymbolGroup::Pulloff, x,
                                        width, height++));
            }
        }
    }

    // Compute the overall spacing needed below the tab staff.
    int maxHeight = 0;
    BOOST_FOREACH(const SymbolGroup &group, myTabStaffBelowSymbols)
    {
        maxHeight = std::max(maxHeight, group.getHeight());
    }

    myTabStaffBelowSpacing = maxHeight * TAB_SYMBOL_SPACING;
}


SymbolGroup::SymbolGroup(const Position &position,
                         SymbolGroup::SymbolType symbol,
                         double x, double width, double height)
    : myPosition(&position),
      mySymbolType(symbol),
      myX(x),
      myWidth(width),
      myHeight(height)
{
}

const std::vector<SymbolGroup> &LayoutInfo::getTabStaffBelowSymbols() const
{
    return myTabStaffBelowSymbols;
}

SymbolGroup::SymbolType SymbolGroup::getSymbolType() const
{
    return mySymbolType;
}

double SymbolGroup::getX() const
{
    return myX;
}

double SymbolGroup::getWidth() const
{
    return myWidth;
}

int SymbolGroup::getHeight() const
{
    return myHeight;
}
