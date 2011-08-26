#include "edittrackshown.h"

#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/system.h>
#include <powertabdocument/staff.h>

EditTrackShown::EditTrackShown(Score* score, uint32_t trackNumber,
                               bool trackShown) :
    score(score), trackNumber(trackNumber), trackShown(trackShown)
{
    Q_ASSERT(score->IsValidGuitarIndex(trackNumber));

    if (trackShown)
    {
        setText(tr("Show Track"));
    }
    else
    {
        setText(tr("Hide Track"));
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

    emit triggered();
}
