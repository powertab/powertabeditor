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
  
#include "layout.h"

#include "score.h"
#include "system.h"
#include "staff.h"
#include "barline.h"
#include "position.h"
#include "guitar.h"
#include "notestem.h"

#include <limits>
#include <algorithm>
#include <numeric>
#include <map>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <iostream>

namespace {
const boost::array<Layout::SymbolType, 7> symbolsBelowTabStaff = {{
    Layout::SymbolPickStrokeDown, Layout::SymbolPickStrokeUp, Layout::SymbolTap,
    Layout::SymbolHammerOnPullOff, Layout::SymbolSlide,
    Layout::SymbolTappedHarmonic, Layout::SymbolArtificialHarmonic
}};
}

/// Adjust the spacing around the standard notation staff, depending on if any
/// notes are located above/below the staff.
void Layout::CalculateStdNotationHeight(Score* score, boost::shared_ptr<System> system)
{
    std::vector<System::BarlineConstPtr> barlines;
    system->GetBarlines(barlines);

    std::vector<NoteStem> stems;

    // idea: find the highest and lowest note of each staff (this requires a lot of loops ...)
    for (size_t staffIndex = 0; staffIndex < system->GetStaffCount(); ++staffIndex)
    {
        double maxNoteLocation = std::numeric_limits<double>::min();
        double minNoteLocation = std::numeric_limits<double>::max();

        System::StaffPtr staff = system->GetStaff(staffIndex);

        const Tuning& tuning = score->GetGuitar(staffIndex)->GetTuning();

        // need to find the range of notes one bar at a time, since key signatures can change the standard notation
        for (size_t barlineIndex = 0; barlineIndex < barlines.size() - 1; ++barlineIndex)
        {
            const KeySignature& activeKeySig = barlines[barlineIndex]->GetKeySignature();

            // check through all notes in all voices and positions
            for (size_t voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
            {
                std::vector<Position*> positions;
                staff->GetPositionsInRange(positions, voice, barlines[barlineIndex]->GetPosition(),
                                           barlines[barlineIndex+1]->GetPosition() - 1);

                for (size_t i = 0; i < positions.size(); i++)
                {
                    const Position* pos = positions[i];
                    std::vector<int> noteLocations;

                    for (size_t j = 0; j < pos->GetNoteCount(); j++)
                    {
                        const double noteLocation = staff->GetNoteLocation(pos->GetNote(j), activeKeySig, tuning) *
                                0.5 * Staff::STD_NOTATION_LINE_SPACING;

                        maxNoteLocation = std::max(maxNoteLocation, noteLocation);
                        minNoteLocation = std::min(minNoteLocation, noteLocation);

                        noteLocations.push_back(noteLocation);
                    }

                    if (NoteStem::needsStem(pos))
                    {
                        stems.push_back(NoteStem(pos, 0, noteLocations, 0, 0));
                    }
                }
            }
        }

        // We also need to make sure there is enough space for the beaming. The
        // max/min comparison above is still necessary though, since e.g. whole
        // notes are not beamed.
        std::vector<NoteStem> currentStemGroup;
        BOOST_FOREACH(const NoteStem& stem, stems)
        {
            currentStemGroup.push_back(stem);

            if (stem.position->IsBeamEnd() ||
                (!stem.position->IsBeamStart() && currentStemGroup.size() == 1))
            {
                NoteStem::StemDirection direction = NoteStem::setStemDirection(
                            currentStemGroup);

                if (direction == NoteStem::StemUp)
                {
                    NoteStem stem = NoteStem::findHighestStem(currentStemGroup);
                    minNoteLocation = std::min(minNoteLocation,
                                               stem.stemTop - stem.stemSize());
                }
                else // stem down
                {
                    NoteStem stem = NoteStem::findLowestStem(currentStemGroup);
                    maxNoteLocation = std::max(maxNoteLocation,
                                            stem.stemBottom + stem.stemSize());
                }

                currentStemGroup.clear();
            }
        }

        // Add spacing if the note is above the staff (height 0).
        staff->SetStandardNotationStaffAboveSpacing(-std::min(0.0, minNoteLocation) +
                                                    Staff::STAFF_BORDER_SPACING);

        // Add spacing if the lowest note is below "middle C" on the staff.
        const double BOTTOM_BOUNDARY = Staff::STD_NOTATION_STAFF_TYPE *
                Staff::STD_NOTATION_LINE_SPACING;
        staff->SetStandardNotationStaffBelowSpacing(
                    std::max(maxNoteLocation, BOTTOM_BOUNDARY) - BOTTOM_BOUNDARY);
    }
}

namespace
{
bool compareHeightOfGroup(const Layout::SymbolGroup& group1, const Layout::SymbolGroup& group2)
{
    return group1.height < group2.height;
}

int maxHeightOfSymbolGroups(const std::vector<Layout::SymbolGroup>& symbolGroups)
{
    int maxHeight = 0;

    if (!symbolGroups.empty())
    {
        maxHeight = std::max_element(symbolGroups.begin(), symbolGroups.end(), compareHeightOfGroup)->height;
    }

    return maxHeight;
}
}

/// Calculates the spacing below the tab staff, for indicating hammerons, pick strokes, taps, etc.
/// The symbols are "stacked" on each other, so the height of the spacing is equal to the height
/// of the largest stack
void Layout::CalculateTabStaffBelowSpacing(boost::shared_ptr<const System> system,
                                           boost::shared_ptr<Staff> staff)
{
    std::vector<SymbolGroup> symbolGroups;
    CalculateTabStaffBelowLayout(symbolGroups, system, staff);

    staff->SetTablatureStaffBelowSpacing(maxHeightOfSymbolGroups(symbolGroups) * Staff::TAB_SYMBOL_HEIGHT);
}

/// Similar to Layout::CalculateTabStaffBelowSpacing, except for symbols displayed between the
/// tab staff and standard notation staff. However, some of the symbols used here do not belong
/// to Position objects (such as dynamics)
void Layout::CalculateSymbolSpacing(const Score* score, boost::shared_ptr<System> system,
                                    boost::shared_ptr<Staff> staff)
{
    std::vector<SymbolGroup> symbolGroups;
    CalculateSymbolLayout(symbolGroups, score, system, staff);

    staff->SetSymbolSpacing(maxHeightOfSymbolGroups(symbolGroups) * Staff::TAB_SYMBOL_HEIGHT);
}

namespace
{
/// Updates the height, by inserting a symbol group spanning the left and right indices
/// @return The height of the symbol group that was inserted
int updateHeightMap(std::vector<int>& heightMap, size_t left, size_t right, int symbolHeight)
{
    const int height = *std::max_element(heightMap.begin() + left,
                                         heightMap.begin() + right) + symbolHeight;
    std::fill_n(heightMap.begin() + left, right - left, height);

    return height;
}

int symbolHeight(Layout::SymbolType symbolType)
{
    return (symbolType == Layout::SymbolBend) ? 3 : 1;
}

}


/// Compute the layout of symbols between the standard notation and tab staves
/// Returns a list of all symbol groups and their locations (for example, consecutive notes with vibrato
/// will have their vibrato symbols grouped together, forming a single vibrato symbol spanning the notes)
void Layout::CalculateSymbolLayout(std::vector<Layout::SymbolGroup>& symbolGroups,
                                   const Score* score, boost::shared_ptr<const System> system,
                                   boost::shared_ptr<const Staff> staff)
{
    std::vector<std::vector<SymbolType> > symbolMap;

    // construct a 2D array, storing the symbols that will be displayed for each position in the staff
    for (uint32_t posIndex = 0; posIndex < staff->GetPositionCount(0); posIndex++)
    {
        const Position* pos = staff->GetPosition(0, posIndex);

        std::vector<SymbolType> symbols;
        symbols.reserve(12);

        // bends must be drawn lower than any other symbol, since they connect with the tab notes
        symbols.push_back(pos->HasNoteWithBend() ? SymbolBend : NoSymbol);

        symbols.push_back(pos->HasLetRing() ? SymbolLetRing : NoSymbol);
        symbols.push_back(pos->HasVolumeSwell() ? SymbolVolumeSwell : NoSymbol);
        symbols.push_back(pos->HasVibrato() ? SymbolVibrato : NoSymbol);
        symbols.push_back(pos->HasWideVibrato() ? SymbolWideVibrato : NoSymbol);
        symbols.push_back(pos->HasPalmMuting() ? SymbolPalmMuting : NoSymbol);
        symbols.push_back(pos->HasTremoloPicking() ? SymbolTremoloPicking : NoSymbol);
        symbols.push_back(pos->HasTremoloBar() ? SymbolTremoloBar : NoSymbol);
        symbols.push_back(pos->HasNoteWithTrill() ? SymbolTrill : NoSymbol);
        symbols.push_back(pos->HasNoteWithNaturalHarmonic() ? SymbolNaturalHarmonic : NoSymbol);
        symbols.push_back(pos->HasNoteWithArtificialHarmonic() ? SymbolArtificialHarmonic : NoSymbol);

        // check for a dynamic at this location
        {
            const uint32_t systemIndex = score->FindSystemIndex(system);
            const uint32_t staffIndex = system->FindStaffIndex(staff);
            if (score->FindDynamic(systemIndex, staffIndex, pos->GetPosition()) != Score::DynamicPtr())
            {
                symbols.push_back(SymbolDynamic);
            }
            else
            {
                symbols.push_back(NoSymbol);
            }
        }

        symbolMap.push_back(symbols);
    }

    // After finding the symbols at each position, we need to group together symbols from neighbouring positions
    // to form larger groups (such as palm muting, vibrato, let ring, etc).
    // We also need to arrange the symbol groups so that they do not overlap (the groups stack on top of each other)

    // stores the highest occupied vertical location at each position index
    std::vector<int> heightMap(symbolMap.size(), 0);

    if (symbolMap.empty())
    {
        return;
    }

    // go through each symbol, and then iterate through the positions and group together symbols (like consecutive notes with vibrato)
    for (size_t symbol = 0; symbol < symbolMap.at(0).size(); symbol++)
    {
        SymbolType currentSymbolType = NoSymbol;
        size_t leftPosIndex = 0;

        for (size_t posIndex = 0; posIndex < symbolMap.size(); posIndex++)
        {
            SymbolType symbolType = symbolMap[posIndex][symbol];

            // if we've reached the end of a symbol group ...
            if (symbolType != currentSymbolType ||
                symbolType == SymbolTrill || // don't group consecutive occurrences of these symbols together
                symbolType == SymbolTremoloPicking || symbolType == SymbolTremoloBar ||
                symbolType == SymbolVolumeSwell || symbolType == SymbolDynamic ||
                symbolType == SymbolBend)
            {
                // record the symbol group and calculate its location
                if (currentSymbolType != NoSymbol)
                {
                    const size_t rightPosIndex = posIndex;

                    const int height = updateHeightMap(heightMap, leftPosIndex, rightPosIndex, symbolHeight(currentSymbolType));

                    const int leftX = system->GetPositionX(staff->GetPosition(0, leftPosIndex)->GetPosition());
                    int rightX = system->GetPositionX(staff->GetPosition(0, rightPosIndex)->GetPosition());

                    // special case: tremolo bar events and volume swells have a duration which can stretch
                    // over several following notes
                    if (currentSymbolType == SymbolTremoloBar || currentSymbolType == SymbolVolumeSwell)
                    {
                        uint8_t duration = 0;
                        const Position* leftPosition = staff->GetPosition(0, leftPosIndex);

                        // get the duration of the tremolo bar dive or volume swell
                        if (currentSymbolType == SymbolTremoloBar)
                        {
                            uint8_t type = 0, pitch = 0;
                            leftPosition->GetTremoloBar(type, duration, pitch);
                        }
                        else if (currentSymbolType == SymbolVolumeSwell)
                        {
                            uint8_t startVolume = 0, endVolume = 0;
                            leftPosition->GetVolumeSwell(startVolume, endVolume, duration);
                        }

                        // if the volume swell or tremolo bar extends onto the next system,
                        // just display until the end of this staff
                        if (!staff->IsValidPositionIndex(0, leftPosIndex + duration))
                        {
                            rightX = system->GetPositionX(system->GetPositionCount() - 1);
                        }
                        else
                        {
                            rightX = system->GetPositionX(staff->GetPosition(0, leftPosIndex + duration)->GetPosition() + 1);
                        }
                    }

                    symbolGroups.push_back(SymbolGroup(leftPosIndex, leftX, rightX - leftX, height, currentSymbolType));
                }

                leftPosIndex = posIndex;
                currentSymbolType = symbolType;
            }
        }

        // if there is a symbol group that stretched to the end of the staff, add it
        if (currentSymbolType != NoSymbol)
        {
            const int leftX = system->GetPositionX(staff->GetPosition(0, leftPosIndex)->GetPosition());
            const int rightX = system->GetPositionX(system->GetPositionCount() - 1);
            const int height = updateHeightMap(heightMap, leftPosIndex, symbolMap.size() - 1, symbolHeight(currentSymbolType));
            symbolGroups.push_back(SymbolGroup(leftPosIndex, leftX, rightX - leftX, height, currentSymbolType));
        }
    }
}

/// Calculates the layout of symbols displayed below the tab staff (horizontal and vertical positioning)
/// There are no symbols which are grouped with neighbouring symbols, making the function significantly simpler
/// than the Layout::CalculateSymbolLayout function
void Layout::CalculateTabStaffBelowLayout(std::vector<Layout::SymbolGroup>& symbolGroups,
                                          boost::shared_ptr<const System> system,
                                          boost::shared_ptr<const Staff> staff)
{
    for (uint32_t posIndex = 0; posIndex < staff->GetPositionCount(0); posIndex++)
    {
        const Position* pos = staff->GetPosition(0, posIndex);
        int height = 1;

        SymbolGroup symbolGroup(pos->GetPosition(), system->GetPositionX(pos->GetPosition()),
                                system->GetPositionSpacing(), 0, NoSymbol);

        boost::array<bool, 7> enabledSymbols = {{
            pos->HasPickStrokeDown(),
            pos->HasPickStrokeUp(),
            pos->HasTap(),
            pos->HasNoteWithHammeronOrPulloff(),
            pos->HasNoteWithSlide(),
            pos->HasNoteWithTappedHarmonic(),
            pos->HasNoteWithArtificialHarmonic()
        }};

        // for each symbol that is enabled, add a corresponding SymbolGroup to the layout
        for (size_t i = 0; i < enabledSymbols.size(); i++)
        {
            if (enabledSymbols[i])
            {
                symbolGroup.symbolType = symbolsBelowTabStaff[i];
                symbolGroup.height = height;
                height++;

                symbolGroups.push_back(symbolGroup);
            }
        }
    }
}

Layout::SymbolGroup::SymbolGroup(int leftPosIndex, int left, int width, int height, Layout::SymbolType type) :
    leftPosIndex(leftPosIndex),
    leftX(left), width(width),
    height(height), symbolType(type)
{
}

/// Auto-adjust spacing of notes in the system
void Layout::FormatSystem(boost::shared_ptr<System> system)
{
    // FIXME - this function doesn't properly adjust the locations of tempo
    // markers, chord names, etc.

    // Ensure that the end bar is farther than any other position (needed so
    // that GetPositionsInRange() doesn't miss any positions).
    system->GetEndBar()->SetPosition(system->GetMaxPosition() + 1);

    std::vector<System::BarlinePtr> barlines;
    system->GetBarlines(barlines);

    // stores the new position of each bar, plus the new positions of all notes in the bar
    // for each staff
    std::vector<std::pair<int, std::vector<std::vector<int> > > > newBarPositions;

    int prevBarPos = 0;
    int nextBarPos = 0;

    for (size_t i = 0; i < barlines.size() - 1; i++)
    {
        System::BarlinePtr leftBar = barlines[i];
        System::BarlinePtr rightBar = barlines[i+1];

        std::vector<std::vector<int> > newPositionsForStaves;

        nextBarPos += 1;

        for (size_t j = 0; j < system->GetStaffCount(); j++)
        {
            System::StaffPtr staff = system->GetStaff(j);

            std::vector<Position*> positions;
            staff->GetPositionsInRange(positions, 0, leftBar->GetPosition(), rightBar->GetPosition() - 1);

            if (positions.empty())
                break;

            std::vector<int> newLocations;
            newLocations.push_back(prevBarPos);

            // Assign a spacing of 1 for eighth notes and lower, 2 for quarter notes, 4 for
            // half notes, etc
            for (size_t k = 0; k < positions.size(); k++)
            {
                double spacing = positions[k]->GetDuration();

                if (spacing < 1)
                    spacing = 1;
                else
                    spacing *= 2;

                newLocations.push_back(spacing);
            }

            // convert into successive position indices, ie [0,1,2,1,2] -> [0,1,3,4,6]
            std::partial_sum(newLocations.begin(), newLocations.end(), newLocations.begin());

            nextBarPos = std::max(nextBarPos, newLocations.back());
            newLocations.pop_back();

            newPositionsForStaves.push_back(newLocations);
        }

        prevBarPos = nextBarPos + 1;

        newBarPositions.push_back(std::make_pair(nextBarPos,
                                                 newPositionsForStaves));
    }

    std::map<int, int> currentPosIndexForStaves;

    // now, set the new locations for the bars and notes
    for (size_t i = 0; i < newBarPositions.size(); i++)
    {
        barlines[i + 1]->SetPosition(newBarPositions[i].first);

        const std::vector<std::vector<int> >& newPositionsForStaves = newBarPositions[i].second;

        for (size_t j = 0; j < newPositionsForStaves.size(); j++)
        {
            const std::vector<int>& newPositions = newPositionsForStaves[j];
            System::StaffPtr staff = system->GetStaff(j);

            for (size_t k = 0; k < newPositions.size(); k++)
            {
                staff->GetPosition(0, currentPosIndexForStaves[j] + k)->SetPosition(newPositions[k]);
            }

            currentPosIndexForStaves[j] += newPositions.size();
        }
    }

    // Set the spacing as large as possible.
    const int availableWidth = system->GetRect().GetWidth() -
            system->GetFirstPositionX() -
            system->GetCumulativeInternalKeyAndTimeSignatureWidth();

    // Add some padding.
    const int numPositions = system->GetMaxPosition() + 2;

    system->SetPositionSpacing(availableWidth / numPositions);
}

/// Converts hammerons to pulloffs (or vice versa) if necessary
/// This is useful when importing files from other formats (Guitar Pro in particular)
void Layout::FixHammerons(boost::shared_ptr<Staff> staff)
{
    for (size_t voice = 0; voice < Staff::NUM_STAFF_VOICES; ++voice)
    {
        for (size_t i = 0; i < staff->GetPositionCount(voice); ++i)
        {
            const Position* pos = staff->GetPosition(voice, i);

            for (size_t j = 0; j < pos->GetNoteCount(); j++)
            {
                Note* note = pos->GetNote(j);

                if (note->HasHammerOn() && staff->CanPullOff(pos, note))
                {
                    note->SetPullOff(true);
                }
                else if (note->HasPullOff() && staff->CanHammerOn(pos, note))
                {
                    note->SetHammerOn(true);
                }
            }
        }
    }
}
