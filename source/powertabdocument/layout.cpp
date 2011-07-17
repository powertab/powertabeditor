#include "layout.h"

#include "score.h"
#include "system.h"
#include "staff.h"
#include "barline.h"
#include "position.h"
#include "guitar.h"

#include <limits>
#include <algorithm>

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

/// Calculates the spacing below the tab staff, for indicating hammerons, pick strokes, taps, etc.
/// The symbols are "stacked" on each other, so the height of the spacing is equal to the height
/// of the largest stack
void Layout::CalculateTabStaffBelowSpacing(std::shared_ptr<Staff> staff)
{
    int maxHeight = 0;

    for (uint32_t voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        for (uint32_t posIndex = 0; posIndex < staff->GetPositionCount(voice); posIndex++)
        {
            const Position* pos = staff->GetPosition(voice, posIndex);

            // count the number of symbols that will be displayed below the staff, for this position
            std::vector<bool> symbols = {
                pos->HasPickStrokeDown(), pos->HasPickStrokeUp(), pos->HasTap(),
                pos->HasNoteWithHammeronOrPulloff(), pos->HasNoteWithSlide(),
                pos->HasNoteWithTappedHarmonic(), pos->HasNoteWithArtificialHarmonic()
            };

            maxHeight = std::max(maxHeight, std::count(symbols.begin(), symbols.end(), true));
        }
    }

    staff->SetTablatureStaffBelowSpacing(maxHeight * Staff::TAB_SYMBOL_HEIGHT);
}

/// Similar to Layout::CalculateTabStaffBelowSpacing, except for symbols displayed between the
/// tab staff and standard notation staff. However, some of the symbols used here do not belong
/// to Position objects (such as dynamics)
void Layout::CalculateSymbolSpacing(const Score* score, std::shared_ptr<System> system,
                                    std::shared_ptr<Staff> staff)
{
    int maxHeight = 0;

    for (uint32_t voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        for (uint32_t posIndex = 0; posIndex < staff->GetPositionCount(voice); posIndex++)
        {
            const Position* pos = staff->GetPosition(voice, posIndex);

            std::vector<bool> symbols = {
                pos->HasLetRing(), pos->HasVolumeSwell(), pos->HasVibrato(),
                pos->HasWideVibrato(), pos->HasPalmMuting(), pos->HasTremoloPicking(),
                pos->HasTremoloBar(), pos->HasNoteWithTrill(),
                pos->HasNoteWithNaturalHarmonic(), pos->HasNoteWithArtificialHarmonic()
            };

            // check for a dynamic at this location
            {
                const uint32_t systemIndex = score->FindSystemIndex(system);
                const uint32_t staffIndex = system->FindStaffIndex(staff);
                symbols.push_back(score->FindDynamic(systemIndex, staffIndex, posIndex) != Score::DynamicPtr());
            }

            maxHeight = std::max(maxHeight, std::count(symbols.begin(), symbols.end(), true));
        }
    }

    staff->SetSymbolSpacing(maxHeight * Staff::TAB_SYMBOL_HEIGHT);
}
