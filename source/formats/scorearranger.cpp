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
  
#include "scorearranger.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/layout.h>
#include <powertabdocument/tempomarker.h>

#include <algorithm>
#include <boost/make_shared.hpp>

namespace
{
/// In Guitar Pro, repeat ends are stored with the current barline, not with the next barline
/// This method adjusts that by shifting repeat ends forward
/// TODO - need to deal with consecutive pairs of repeat start/end bars
void fixRepeats(Score *score)
{
    for (size_t i = 0; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr currentSystem = score->GetSystem(i);
        std::vector<System::BarlinePtr> barlines;
        currentSystem->GetBarlines(barlines);

        for (size_t j = 0; j < barlines.size() - 1; j++)
        {
            System::BarlinePtr currentBarline = barlines[j];
            System::BarlinePtr nextBarline = barlines[j+1];

            if (currentBarline->GetRepeatCount() >= Barline::MIN_REPEAT_COUNT &&
                    !currentBarline->IsRepeatEnd())
            {
                nextBarline->SetType(Barline::repeatEnd);
                nextBarline->SetRepeatCount(currentBarline->GetRepeatCount());
            }
        }
    }
}

/// Shift double bars and free-time bars over by one, due to the difference
/// between how barline types are handled in PowerTab and Guitar Pro.
void fixDoubleAndFreeTimeBars(Score *score)
{
    for (size_t i = 0; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr currentSystem = score->GetSystem(i);
        std::vector<System::BarlinePtr> barlines;
        currentSystem->GetBarlines(barlines);

        assert(!barlines.empty());

        for (int j = barlines.size() - 1; j >= 1; --j)
        {
            System::BarlinePtr prevBarline = barlines[j-1];
            System::BarlinePtr currentBarline = barlines[j];

            if (prevBarline->IsDoubleBar() || prevBarline->IsFreeTimeBar())
            {
                currentBarline->SetType(prevBarline->GetType());
                prevBarline->SetType(Barline::bar);
            }
        }
    }
}
}

/// Arranges the list of bars into systems (useful for importing from other file formats
/// that don't provide formatting information)
void arrangeScore(Score* score, const std::vector<BarData>& bars,
                  bool fixBarlineTypes)
{
    // Use a slightly smaller position spacing than the default in order to reduce the number of systems needed
    const uint8_t DEFAULT_POSITION_SPACING = 15;

    // record the number of strings that will be in each staff
    std::vector<uint8_t> staffSizes;
    std::vector<bool> visibleStaves;
    for (uint32_t i = 0; i < score->GetGuitarCount(); i++)
    {
        staffSizes.push_back(score->GetGuitar(i)->GetStringCount());
        visibleStaves.push_back(score->GetGuitar(i)->IsShown());
    }

    // set up initial system
    Score::SystemPtr currentSystem = boost::make_shared<System>();
    currentSystem->Init(staffSizes, visibleStaves, false);
    currentSystem->SetPositionSpacing(DEFAULT_POSITION_SPACING);
    score->InsertSystem(currentSystem, 0);

    std::vector<BarData>::const_iterator currentBar = bars.begin();
    uint32_t lastBarlinePos = 0;

    for (uint32_t i = 0; i < bars.size(); i++)
    {
        const std::vector<std::vector<Position*> >& positionLists = bars[i].positionLists;
        size_t largestMeasure = 0;

        for (size_t j = 0; j < positionLists.size(); j++)
        {
            largestMeasure = std::max(positionLists[j].size(), largestMeasure);
        }

        // check if we need to jump to a new system (measure is too large)
        if (!currentSystem->IsValidPosition(lastBarlinePos + largestMeasure + 1))
        {
            currentSystem = boost::make_shared<System>();
            currentSystem->Init(staffSizes, visibleStaves, false);
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

                // Show the key signature at the start of each system.
                currentBar->barline->GetKeySignature().SetShown(true);
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

            // Insert tempo marker if necessary.
            if (currentBar->tempoMarker)
            {
                currentBar->tempoMarker->SetSystem(score->GetSystemCount() - 1);
                currentBar->tempoMarker->SetPosition(currentBar->barline->GetPosition());
                score->InsertTempoMarker(currentBar->tempoMarker);
            }

            lastBarlinePos += largestMeasure + 1;
            ++currentBar;
        }
    }

    // Show the final barline.
    score->GetSystem(score->GetSystemCount() - 1)->GetEndBar()->SetType(
                Barline::doubleBarFine);

    // calculate the beaming for all notes, and calculate the layout of the systems
    for (size_t i = 0; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr system = score->GetSystem(i);
        system->CalculateBeamingForStaves();
        score->UpdateSystemHeight(system);
        Layout::FormatSystem(system);

        for (size_t i = 0; i < system->GetStaffCount(); i++)
        {
            Layout::FixHammerons(system->GetStaff(i));
        }
    }

    score->FormatRehearsalSigns();

    if (fixBarlineTypes)
    {
        fixRepeats(score);
        fixDoubleAndFreeTimeBars(score);
    }
}
