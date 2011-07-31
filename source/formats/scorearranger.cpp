#include "scorearranger.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/layout.h>

#include <algorithm>

/// Arranges the list of bars into systems (useful for importing from other file formats
/// that don't provide formatting information)
void arrangeScore(Score* score, const std::vector<BarData>& bars)
{
    // Use a slightly smaller position spacing than the default in order to reduce the number of systems needed
    const uint8_t DEFAULT_POSITION_SPACING = 15;

    // record the number of strings that will be in each staff
    std::vector<uint8_t> staffSizes;
    for (uint32_t guitar = 0; guitar < score->GetGuitarCount(); guitar++)
    {
        staffSizes.push_back(score->GetGuitar(guitar)->GetStringCount());
    }

    // set up initial system
    Score::SystemPtr currentSystem = std::make_shared<System>();
    currentSystem->Init(staffSizes);
    currentSystem->SetPositionSpacing(DEFAULT_POSITION_SPACING);
    score->InsertSystem(currentSystem, 0);

    auto currentBar = bars.begin();
    uint32_t lastBarlinePos = 0;

    for (uint32_t i = 0; i < bars.size(); i++)
    {
        const std::vector<std::vector<Position*> >& positionLists = bars[i].positionLists;
        size_t largestMeasure = 0;

        for (auto j = positionLists.begin(); j != positionLists.end(); ++j)
        {
            largestMeasure = std::max(j->size(), largestMeasure);
        }

        // check if we need to jump to a new system (measure is too large)
        if (!currentSystem->IsValidPosition(lastBarlinePos + largestMeasure + 1))
        {
            currentSystem = std::make_shared<System>();
            currentSystem->Init(staffSizes);
            currentSystem->SetPositionSpacing(DEFAULT_POSITION_SPACING);

            // adjust height to be below the previous system
            const Rect prevRect = score->GetSystem(score->GetSystemCount() - 1)->GetRect();
            Rect currentRect = currentSystem->GetRect();
            currentRect.SetTop(prevRect.GetBottom() + Score::SYSTEM_SPACING);
            currentSystem->SetRect(currentRect);

            score->InsertSystem(currentSystem, score->GetSystemCount());
            lastBarlinePos = 0;

            // ensure that there is enough space in the staff for all notes of the measure
            if (!currentSystem->IsValidPosition(largestMeasure))
            {
                currentSystem->SetPositionSpacing(System::MIN_POSITION_SPACING);
            }
        }

        // insert positions
        for (uint32_t track = 0; track < score->GetGuitarCount(); track++)
        {
            const std::vector<Position*>& positionList = positionLists.at(track);
            System::StaffPtr currentStaff = currentSystem->GetStaff(track);

            for (uint32_t posIndex = 0; posIndex < positionList.size(); posIndex++)
            {
                Position* currentPos = positionList.at(posIndex);
                currentPos->SetPosition(lastBarlinePos + posIndex);
                currentStaff->InsertPosition(0, currentPos);
            }

            currentStaff->CalculateClef(score->GetGuitar(track)->GetTuning());
        }

        // insert barline
        if (currentBar != bars.end())
        {
            if (lastBarlinePos == 0)
            {
                currentSystem->SetStartBar(currentBar->barline);
                currentBar->barline->SetPosition(0);
            }
            else
            {
                currentBar->barline->SetPosition(lastBarlinePos - 1);
                currentSystem->InsertBarline(currentBar->barline);
            }

            // insert alternate ending if necessary
            if (currentBar->altEnding)
            {
                currentBar->altEnding->SetSystem(score->GetSystemCount() - 1);
                currentBar->altEnding->SetPosition(currentBar->barline->GetPosition());
                score->InsertAlternateEnding(currentBar->altEnding);
            }

            lastBarlinePos += largestMeasure + 1;
            ++currentBar;
        }
    }

    // calculate the beaming for all notes, and calculate the layout of the systems
    for (size_t i = 0; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr system = score->GetSystem(i);
        system->CalculateBeamingForStaves();
        score->UpdateSystemHeight(system);
        Layout::FormatSystem(system);
    }
}
