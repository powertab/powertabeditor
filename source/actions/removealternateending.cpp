#include "removealternateending.h"

#include <powertabdocument/score.h>

RemoveAlternateEnding::RemoveAlternateEnding(Score* score,
                                             std::shared_ptr<AlternateEnding> altEnding) :
    score(score),
    altEnding(altEnding)
{
    setText(QObject::tr("Remove Repeat Ending"));
}

void RemoveAlternateEnding::redo()
{
    score->RemoveAlternateEnding(altEnding);
}

void RemoveAlternateEnding::undo()
{
    score->InsertAlternateEnding(altEnding);
}
