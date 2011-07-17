#include "layout.h"

#include "score.h"
#include "system.h"
#include "staff.h"
#include "barline.h"
#include "position.h"
#include "guitar.h"

#include <limits>
#include <algorithm>
#include <map>
#include <boost/foreach.hpp>

/// Adjust the spacing around the standard notation staff, depending on if any notes are located above/below the staff
void Layout::CalculateStdNotationHeight(Score* score, std::shared_ptr<System> system)
{
    std::vector<System::BarlineConstPtr> barlines;
    system->GetBarlines(barlines);

    // idea: find the highest and lowest note of each staff (this requires a lot of loops ...)
    for (size_t staffIndex = 0; staffIndex < system->GetStaffCount(); ++staffIndex)
    {
        int maxNoteLocation = std::numeric_limits<int>::min();
        int minNoteLocation = std::numeric_limits<int>::max();
        bool noteEncountered = false;

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
                                           barlines[barlineIndex+1]->GetPosition());

                for (size_t i = 0; i < positions.size(); i++)
                {
                    const Position* pos = positions[i];

                    for (size_t j = 0; j < pos->GetNoteCount(); j++)
                    {
                        const int noteLocation = staff->GetNoteLocation(pos->GetNote(j), activeKeySig, tuning);

                        maxNoteLocation = std::max(maxNoteLocation, noteLocation);
                        minNoteLocation = std::min(minNoteLocation, noteLocation);

                        noteEncountered = true;
                    }
                }
            }
        }

        // since we use +/- INT_MAX for correctly computing the minimum/maximum, we don't want to accidently
        // set the staff spacing to a very large/small value
        if (noteEncountered)
        {
            if (minNoteLocation < 0) // if the highest note is above the staff
            {
                staff->SetStandardNotationStaffAboveSpacing(-minNoteLocation * 0.5 *
                                                            Staff::STD_NOTATION_LINE_SPACING +
                                                            Staff::STAFF_BORDER_SPACING);
            }

            // if the lowest note is below "middle C" on the staff
            const int BOTTOM_BOUNDARY = Staff::STD_NOTATION_STAFF_TYPE * 2;
            if (maxNoteLocation > BOTTOM_BOUNDARY)
            {
                staff->SetStandardNotationStaffBelowSpacing((maxNoteLocation - BOTTOM_BOUNDARY) *
                                                            0.5 * Staff::STD_NOTATION_LINE_SPACING);
            }
        }
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
void Layout::CalculateTabStaffBelowSpacing(std::shared_ptr<const System> system,
                                           std::shared_ptr<Staff> staff)
{
    std::vector<SymbolGroup> symbolGroups = CalculateTabStaffBelowLayout(system, staff);

    staff->SetTablatureStaffBelowSpacing(maxHeightOfSymbolGroups(symbolGroups) * Staff::TAB_SYMBOL_HEIGHT);
}

/// Similar to Layout::CalculateTabStaffBelowSpacing, except for symbols displayed between the
/// tab staff and standard notation staff. However, some of the symbols used here do not belong
/// to Position objects (such as dynamics)
void Layout::CalculateSymbolSpacing(const Score* score, std::shared_ptr<System> system,
                                    std::shared_ptr<Staff> staff)
{
    std::vector<SymbolGroup> symbolGroups = CalculateSymbolLayout(score, system, staff);

    staff->SetSymbolSpacing(maxHeightOfSymbolGroups(symbolGroups) * Staff::TAB_SYMBOL_HEIGHT);
}

namespace
{
/// Updates the height, by inserting a symbol group spanning the left and right indices
/// @return The height of the symbol group that was inserted
int updateHeightMap(std::vector<int>& heightMap, size_t left, size_t right)
{
    const int height = *std::max_element(heightMap.begin() + left,
                                         heightMap.begin() + right) + 1;
    std::fill_n(heightMap.begin() + left, right - left, height);

    return height;
}
}

/// Compute the layout of symbols between the standard notation and tab staves
/// Returns a list of all symbol groups and their locations (for example, consecutive notes with vibrato
/// will have their vibrato symbols grouped together, forming a single vibrato symbol spanning the notes)
std::vector<Layout::SymbolGroup> Layout::CalculateSymbolLayout(const Score* score, std::shared_ptr<const System> system,
                                                               std::shared_ptr<const Staff> staff)
{
    std::vector<std::vector<SymbolType> > symbolMap;

    // construct a 2D array, storing the symbols that will be displayed for each position in the staff
    for (uint32_t posIndex = 0; posIndex < staff->GetPositionCount(0); posIndex++)
    {
        const Position* pos = staff->GetPosition(0, posIndex);

        std::vector<SymbolType> symbols;

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

    std::vector<SymbolGroup> symbolGroups;

    if (symbolMap.empty())
    {
        return symbolGroups;
    }

    // go through each symbol, and then iterate through the positions and group together symbols
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
                symbolType == SymbolVolumeSwell || symbolType == SymbolDynamic)
            {
                // record the symbol group and calculate its location
                if (currentSymbolType != NoSymbol)
                {
                    const size_t rightPosIndex = posIndex;

                    const int height = updateHeightMap(heightMap, leftPosIndex, rightPosIndex);

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
            const int height = updateHeightMap(heightMap, leftPosIndex, symbolMap.size() - 1);
            symbolGroups.push_back(SymbolGroup(leftPosIndex, leftX, rightX - leftX, height, currentSymbolType));
        }
    }

    return symbolGroups;
}

/// Calculates the layout of symbols displayed below the tab staff (horizontal and vertical positioning)
/// There are no symbols which are grouped with neighbouring symbols, making the function significantly simpler
/// than the Layout::CalculateSymbolLayout function
std::vector<Layout::SymbolGroup> Layout::CalculateTabStaffBelowLayout(std::shared_ptr<const System> system,
                                                                      std::shared_ptr<const Staff> staff)
{
    std::vector<SymbolGroup> symbolGroups;

    for (uint32_t posIndex = 0; posIndex < staff->GetPositionCount(0); posIndex++)
    {
        const Position* pos = staff->GetPosition(0, posIndex);

        SymbolGroup symbolGroup(pos->GetPosition(), system->GetPositionX(pos->GetPosition()),
                                system->GetPositionSpacing(), 0, NoSymbol);

        // associate the symbol type with each enabled/disabled symbol
        std::map<bool, SymbolType> symbols = {
            {pos->HasPickStrokeDown(), SymbolPickStrokeDown},
            {pos->HasPickStrokeUp(), SymbolPickStrokeUp},
            {pos->HasTap(), SymbolTap},
            {pos->HasNoteWithHammeronOrPulloff(), SymbolHammerOnPullOff},
            {pos->HasNoteWithSlide(), SymbolSlide},
            {pos->HasNoteWithTappedHarmonic(), SymbolTappedHarmonic},
            {pos->HasNoteWithArtificialHarmonic(), SymbolArtificialHarmonic}
        };

        // for each symbol that is enabled, add a corresponding SymbolGroup to the layout
        BOOST_FOREACH(const auto& symbol, symbols)
        {
            if (symbol.first == true)
            {
                symbolGroup.symbolType = symbol.second;
                symbolGroups.push_back(symbolGroup);
            }
        }
    }

    return symbolGroups;
}

Layout::SymbolGroup::SymbolGroup(int leftPosIndex, int left, int width, int height, Layout::SymbolType type) :
    leftPosIndex(leftPosIndex),
    leftX(left), width(width),
    height(height), symbolType(type)
{
}
