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

#include <score/keysignature.h>
#include <score/score.h>
#include <score/staff.h>
#include <score/system.h>
#include <score/timesignature.h>
#include <score/utils.h>
#include <score/voiceutils.h>
#include <set>

const double LayoutInfo::STAFF_WIDTH = 750;
const int LayoutInfo::NUM_STD_NOTATION_LINES = 5;
const double LayoutInfo::STD_NOTATION_LINE_SPACING = 7;
const double LayoutInfo::STAFF_BORDER_SPACING = 12;
const double LayoutInfo::CLEF_PADDING = 3;
const double LayoutInfo::ACCIDENTAL_WIDTH = 6;
const double LayoutInfo::CLEF_WIDTH = 22;
const double LayoutInfo::SYSTEM_SYMBOL_SPACING = 18;
const double LayoutInfo::MIN_POSITION_SPACING = 3;
const double LayoutInfo::TAB_SYMBOL_SPACING = 10;
const double LayoutInfo::DEFAULT_POSITION_SPACING = 20;

LayoutInfo::LayoutInfo(const Score &score, const System &system, int systemIndex,
                       const Staff &staff, int staffIndex)
    : mySystem(system),
      myStaff(staff),
      myLineSpacing(score.getLineSpacing()),
      myPositionSpacing(0),
      myNumPositions(0),
      myTabStaffBelowSpacing(0),
      myTabStaffAboveSpacing(0),
      myStdNotationStaffAboveSpacing(0),
      myStdNotationStaffBelowSpacing(0)
{
    computePositionSpacing();
    calculateTabStaffBelowLayout();
    calculateTabStaffAboveLayout();

    StdNotationNote::getNotesInStaff(score, system, systemIndex, staff,
                                     staffIndex, *this, myNotes, myStems,
                                     myBeamGroups);

    calculateStdNotationStaffAboveLayout();
    calculateStdNotationStaffBelowLayout();

    // Update the locations of the stems based on the spacing we just computed.
    const double offset = getTopStdNotationLine();
    for (NoteStem &stem : myStems)
    {
        stem.setTop(stem.getTop() + offset);
        stem.setBottom(stem.getBottom() + offset);
    }
}

int LayoutInfo::getStringCount() const
{
    return myStaff.getStringCount();
}

double LayoutInfo::getSystemSymbolSpacing() const
{
    double height = 0;

    for (const Barline &barline : mySystem.getBarlines())
    {
        if (barline.hasRehearsalSign())
        {
            height += SYSTEM_SYMBOL_SPACING;
            break;
        }
    }

    if (!mySystem.getAlternateEndings().empty())
        height += SYSTEM_SYMBOL_SPACING;

    if (!mySystem.getTempoMarkers().empty())
        height += SYSTEM_SYMBOL_SPACING;

    if (!mySystem.getChords().empty())
        height += SYSTEM_SYMBOL_SPACING;

    double directionHeight = 0;
    for (const Direction &direction : mySystem.getDirections())
    {
        directionHeight = std::max(directionHeight,
                                   direction.getSymbols().size() *
                                   SYSTEM_SYMBOL_SPACING);
    }

    height += directionHeight;

    return height;
}

double LayoutInfo::getStaffHeight() const
{
    return myStdNotationStaffAboveSpacing + myStdNotationStaffBelowSpacing +
            myTabStaffAboveSpacing + myTabStaffBelowSpacing +
            STD_NOTATION_LINE_SPACING * (NUM_STD_NOTATION_LINES - 1) +
            (getStringCount() - 1) * getTabLineSpacing() +
            4 * STAFF_BORDER_SPACING;
}

double LayoutInfo::getStdNotationLine(int line) const
{
    return myStdNotationStaffAboveSpacing + STAFF_BORDER_SPACING +
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
    return myLineSpacing;
}

double LayoutInfo::getTabStaffHeight() const
{
    return (getStringCount() - 1) * getTabLineSpacing();
}

double LayoutInfo::getPositionSpacing() const
{
    return myPositionSpacing;
}

int LayoutInfo::getNumPositions() const
{
    return myNumPositions;
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

int LayoutInfo::getPositionFromX(double x) const
{
    if (getPositionX(0) >= x)
        return 0;

    const int maxPosition = getNumPositions() - 2;

    for (int i = 1; i < maxPosition; ++i)
    {
        if (getPositionX(i) >= x)
            return i - 1;
    }

    return maxPosition;
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

double LayoutInfo::getTabStaffBelowSpacing() const
{
    return myTabStaffBelowSpacing;
}

double LayoutInfo::getCumulativeBarlineWidths(int position) const
{
    double width = 0;

    const bool allBarlines = (position == -1);

    for (const Barline &barline : mySystem.getBarlines())
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
    for (const Staff &staff : mySystem.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            for (const Position &position : voice.getPositions())
            {
                myNumPositions = std::max(myNumPositions,
                                          position.getPosition());
            }
        }
    }

    // TODO - include chord text, tempo markers, etc.
    for (const Barline &barline : mySystem.getBarlines())
        myNumPositions = std::max(myNumPositions, barline.getPosition());

    const double availableSpace = STAFF_WIDTH - width;
    myPositionSpacing = availableSpace / (myNumPositions + 2);
}

void LayoutInfo::calculateTabStaffBelowLayout()
{
    for (const Voice &voice : myStaff.getVoices())
    {
        for (const Position &pos : voice.getPositions())
        {
            int height = 1;
            const int position = pos.getPosition();
            const double width = getPositionSpacing();

            if (pos.hasProperty(Position::PickStrokeUp))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::PickStrokeUp, position,
                                        voice, width, height++));
            }

            if (pos.hasProperty(Position::PickStrokeDown))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::PickStrokeDown, position,
                                        voice, width, height++));
            }

            if (pos.hasProperty(Position::Tap) ||
                Utils::hasNoteWithTappedHarmonic(pos))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::Tap, position,
                                        voice, width, height++));
            }

            if (Utils::hasNoteWithArtificialHarmonic(pos))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::ArtificialHarmonic,
                                        position, voice, width, height++));
            }

            if (VoiceUtils::hasNoteWithHammerOn(voice, pos) ||
                Utils::hasNoteWithProperty(pos, Note::HammerOnFromNowhere))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::Hammeron, position,
                                        voice, width, height++));
            }
            else if ((Utils::hasNoteWithProperty(pos, Note::HammerOnOrPullOff) &&
                      !VoiceUtils::hasNoteWithHammerOn(voice, pos)) ||
                     Utils::hasNoteWithProperty(pos, Note::PullOffToNowhere))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::Pulloff, position, voice,
                                        width, height++));
            }

            if (Utils::hasNoteWithProperty(pos, Note::SlideIntoFromAbove) ||
                Utils::hasNoteWithProperty(pos, Note::SlideIntoFromBelow) ||
                Utils::hasNoteWithProperty(pos, Note::ShiftSlide) ||
                Utils::hasNoteWithProperty(pos, Note::LegatoSlide) ||
                Utils::hasNoteWithProperty(pos, Note::SlideOutOfDownwards) ||
                Utils::hasNoteWithProperty(pos, Note::SlideOutOfUpwards))
            {
                myTabStaffBelowSymbols.push_back(
                            SymbolGroup(SymbolGroup::Slide, position, voice,
                                        width, height++));
            }
        }
    }

    // Compute the overall spacing needed below the tab staff.
    myTabStaffBelowSpacing =
            getMaxHeight(myTabStaffBelowSymbols) * TAB_SYMBOL_SPACING;
}

void LayoutInfo::calculateStdNotationStaffAboveLayout()
{
    calculateOctaveSymbolLayout(myStdNotationStaffAboveSymbols, true);

    myStdNotationStaffAboveSpacing =
            getMaxHeight(myStdNotationStaffAboveSymbols) * TAB_SYMBOL_SPACING;

    // Reserve space for notes and their stems.
    double minLocation = std::numeric_limits<double>::max();
    for (const NoteStem &stem: myStems)
        minLocation = std::min(minLocation, stem.getTop());

    myStdNotationStaffAboveSpacing += -std::min(0.0, minLocation);
}

void LayoutInfo::calculateStdNotationStaffBelowLayout()
{
    calculateOctaveSymbolLayout(myStdNotationStaffBelowSymbols, false);

    myStdNotationStaffBelowSpacing =
            getMaxHeight(myStdNotationStaffBelowSymbols) * TAB_SYMBOL_SPACING;

    // Reserve space for notes and their stems.
    double maxLocation = -std::numeric_limits<double>::max();
    for (const NoteStem &stem : myStems)
        maxLocation = std::max(maxLocation, stem.getBottom());

    const double bottomBoundary = 5 * STD_NOTATION_LINE_SPACING;
    myStdNotationStaffBelowSpacing +=
            std::max(bottomBoundary, maxLocation) - bottomBoundary;
}

void LayoutInfo::calculateOctaveSymbolLayout(std::vector<SymbolGroup> &symbols,
                                             bool aboveStaff)
{
    for (const Voice &voice : myStaff.getVoices())
    {
        SymbolGroup::SymbolType currentType = SymbolGroup::NoSymbol;
        int leftPos = 0;

        for (const Position &pos : voice.getPositions())
        {
            SymbolGroup::SymbolType type = SymbolGroup::NoSymbol;

            if (aboveStaff)
            {
                if (Utils::hasNoteWithProperty(pos, Note::Octave8va))
                    type = SymbolGroup::Octave8va;
                else if (Utils::hasNoteWithProperty(pos, Note::Octave15ma))
                    type = SymbolGroup::Octave15ma;
            }
            else
            {
                if (Utils::hasNoteWithProperty(pos, Note::Octave8vb))
                    type = SymbolGroup::Octave8vb;
                else if (Utils::hasNoteWithProperty(pos, Note::Octave15mb))
                    type = SymbolGroup::Octave15mb;
            }

            const bool endOfStaff = (pos == voice.getPositions().back());

            // If we've reached the end of a group or the end of the staff,
            // record the group.
            if (type != currentType || endOfStaff)
            {
                if (currentType != SymbolGroup::NoSymbol)
                {
                    const double left = getPositionX(leftPos);

                    int rightPos = pos.getPosition();
                    if (endOfStaff)
                        rightPos++;
                    const double right = getPositionX(rightPos);

                    symbols.push_back(SymbolGroup(currentType, leftPos, voice,
                                                  right - left, 1));
                }

                leftPos = pos.getPosition();
                currentType = type;
            }
        }
    }
}

namespace {
/// Insert a symbol group spanning the given positions. All of the symbols will
/// be given the same y location.
int addToHeightMap(std::vector<int> &heightMap, int left, int right, int height)
{
    const int newHeight = *std::max_element(heightMap.begin() + left,
                                            heightMap.begin() + right) + height;
    std::fill_n(heightMap.begin() + left, right - left, newHeight);
    return newHeight;
}
}

void LayoutInfo::calculateTabStaffAboveLayout()
{
    // First, allocate spacing for player changes in the system.
    const int staffIndex = std::find(mySystem.getStaves().begin(),
                                      mySystem.getStaves().end(), myStaff) -
                           mySystem.getStaves().begin();

    for (const PlayerChange &change : mySystem.getPlayerChanges())
    {
        if (!change.getActivePlayers(staffIndex).empty())
        {
            myTabStaffAboveSpacing = TAB_SYMBOL_SPACING;
            break;
        }
    }

    typedef std::set<SymbolGroup::SymbolType> SymbolSet;
    // For each position in the staff, build a set of symbols at that position.
    std::vector<SymbolSet> symbolSets(getNumPositions() + 1);

    // Add symbols from each position.
    for (const Voice &voice : myStaff.getVoices())
    {
        for (const Position &pos : voice.getPositions())
        {
            SymbolSet &set = symbolSets.at(pos.getPosition());

            // TODO - handle bends.

            if (pos.hasProperty(Position::LetRing))
                set.insert(SymbolGroup::LetRing);

            // TODO - handle volume swells.

            if (pos.hasProperty(Position::Vibrato))
                set.insert(SymbolGroup::Vibrato);
            if (pos.hasProperty(Position::WideVibrato))
                set.insert(SymbolGroup::WideVibrato);
            if (pos.hasProperty(Position::PalmMuting))
                set.insert(SymbolGroup::PalmMuting);
            if (pos.hasProperty(Position::TremoloPicking))
                set.insert(SymbolGroup::TremoloPicking);

            // TODO - handle tremolo bars.

            if (Utils::hasNoteWithTrill(pos))
                set.insert(SymbolGroup::Trill);
            if (Utils::hasNoteWithProperty(pos, Note::NaturalHarmonic))
                set.insert(SymbolGroup::NaturalHarmonic);
            if (Utils::hasNoteWithArtificialHarmonic(pos))
                set.insert(SymbolGroup::ArtificialHarmonic);

            // If there aren't any symbols, insert a dummy symbol so that this
            // position is treated differently than a completely empty position
            // and breaks any symbol groups.
            if (set.empty())
                set.insert(SymbolGroup::NoSymbol);
        }
    }

    // Add dynamic symbols.
    for (const Dynamic &dynamic : myStaff.getDynamics())
        symbolSets.at(dynamic.getPosition()).insert(SymbolGroup::Dynamic);

    // Now, we need to form symbol groups for symbols such as vibrato or let
    // ring that are used by multiple consecutive notes.
    // We also need to compute y-locations for the groups so that none of them
    // overlap.

    // Stores the highest occupied slot at each position.
    std::vector<int> heightMap(symbolSets.size(), 0);

    for (int symbolIndex = SymbolGroup::LetRing;
         symbolIndex <= SymbolGroup::ArtificialHarmonic; ++symbolIndex)
    {
        SymbolGroup::SymbolType symbol = static_cast<SymbolGroup::SymbolType>(
                    symbolIndex);
        bool inGroup = false;
        int leftPos = 0;

        for (size_t i = 0; i < symbolSets.size(); ++i)
        {
            const SymbolSet &set = symbolSets[i];

            // Skip empty positions.
            if (set.empty())
                continue;

            const bool hasSymbol = set.find(symbol) != set.end();

            // Finish creating a group if we've reached the end of one, or
            // we're handling a symbol that doesn't get grouped.
            if ((!hasSymbol && inGroup) || (hasSymbol && (
                symbol == SymbolGroup::Trill ||
                symbol == SymbolGroup::TremoloPicking ||
                symbol == SymbolGroup::Dynamic)))
            {
                int rightPos = i;

                if (!inGroup)
                {
                    leftPos = i;
                    rightPos = leftPos + 1;
                }

                inGroup = false;

                const int height = addToHeightMap(heightMap, leftPos,
                                                  rightPos, 1);
                const double leftX = getPositionX(leftPos);
                const double rightX = getPositionX(rightPos);

                // TODO - handle special cases for tremolo bars and volume
                // swells.

                myTabStaffAboveSymbols.push_back(
                    SymbolGroup(symbol, leftPos, myStaff.getVoices()[0],
                                rightX - leftX, height));
            }
            // Start a new group.
            else if (hasSymbol && !inGroup)
            {
                leftPos = i;
                inGroup = true;
            }
        }

        // If we were in a symbol group that stretched to the end of the staff,
        // add it.
        if (inGroup)
        {
            const double leftX = getPositionX(leftPos);
            const int rightPos = symbolSets.size() - 1;
            const double rightX = getPositionX(rightPos);
            const int height = addToHeightMap(heightMap, leftPos, rightPos, 1);
            myTabStaffAboveSymbols.push_back(
                SymbolGroup(symbol, leftPos, myStaff.getVoices()[0],
                            rightX - leftX, height));
        }
    }

    // Compute the overall spacing needed below the tab staff.
    myTabStaffAboveSpacing +=
            getMaxHeight(myTabStaffAboveSymbols) * TAB_SYMBOL_SPACING;
}

int LayoutInfo::getMaxHeight(const std::vector<SymbolGroup> &groups)
{
    int maxHeight = 0;

    for (const SymbolGroup &group : groups)
        maxHeight = std::max(maxHeight, group.getHeight());

    return maxHeight;
}

SymbolGroup::SymbolGroup(SymbolGroup::SymbolType symbol, int position,
                         const Voice &voice, double width, int height)
    : mySymbolType(symbol),
      myPosition(position),
      myVoice(voice),
      myWidth(width),
      myHeight(height)
{
}

const std::vector<SymbolGroup> &LayoutInfo::getTabStaffBelowSymbols() const
{
    return myTabStaffBelowSymbols;
}

const std::vector<SymbolGroup> &LayoutInfo::getTabStaffAboveSymbols() const
{
    return myTabStaffAboveSymbols;
}

const std::vector<SymbolGroup> &LayoutInfo::getStdNotationStaffAboveSymbols() const
{
    return myStdNotationStaffAboveSymbols;
}

double LayoutInfo::getStdNotationStaffBelowSpacing() const
{
    return myStdNotationStaffBelowSpacing;
}

const std::vector<SymbolGroup> &LayoutInfo::getStdNotationStaffBelowSymbols() const
{
    return myStdNotationStaffBelowSymbols;
}

const std::vector<StdNotationNote> &LayoutInfo::getStdNotationNotes() const
{
    return myNotes;
}

const std::vector<BeamGroup> &LayoutInfo::getBeamGroups() const
{
    return myBeamGroups;
}

const std::vector<NoteStem> &LayoutInfo::getNoteStems() const
{
    return myStems;
}

SymbolGroup::SymbolType SymbolGroup::getSymbolType() const
{
    return mySymbolType;
}

int SymbolGroup::getPosition() const
{
    return myPosition;
}

const Voice &SymbolGroup::getVoice() const
{
    return myVoice;
}

double SymbolGroup::getWidth() const
{
    return myWidth;
}

int SymbolGroup::getHeight() const
{
    return myHeight;
}
