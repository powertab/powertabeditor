/*
  * Copyright (C) 2012 Cameron White
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

#include "edittrackshown.h"

#include <powertabdocument/guitar.h>
#include <powertabdocument/score.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/system.h>
#include <widgets/mixer/mixer.h>

EditTrackShown::EditTrackShown(Score* score, Mixer* mixer, uint32_t trackNumber,
                               bool trackShown) :
    score(score),
    mixer(mixer),
    trackNumber(trackNumber),
    trackShown(trackShown)
{
    Q_ASSERT(score->IsValidGuitarIndex(trackNumber));

    if (trackShown)
    {
        setText(QObject::tr("Show Track"));
    }
    else
    {
        setText(QObject::tr("Hide Track"));
    }
}

void EditTrackShown::redo()
{
    toggleShown(trackShown);
}

void EditTrackShown::undo()
{
    toggleShown(!trackShown);
}

void EditTrackShown::toggleShown(bool show)
{
    score->GetGuitar(trackNumber)->SetShown(show);

    for (size_t i = 0; i < score->GetSystemCount(); ++i)
    {
        Score::SystemPtr system = score->GetSystem(i);

        system->GetStaff(trackNumber)->SetShown(show);

        const int originalHeight = system->GetRect().GetHeight();
        system->CalculateHeight();
        const int spacingDifference = system->GetRect().GetHeight() - originalHeight;

        score->ShiftFollowingSystems(system, spacingDifference);
    }

    mixer->update();
}
