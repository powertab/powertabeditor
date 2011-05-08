#include "addalternateending.h"

#include <powertabdocument/score.h>

AddAlternateEnding::AddAlternateEnding(Score* score,
                                       std::shared_ptr<AlternateEnding> altEnding) :
    score(score),
    altEnding(altEnding)
{
    setText(QObject::tr("Add Repeat Ending"));
}

void AddAlternateEnding::redo()
{
    score->InsertAlternateEnding(altEnding);
}

void AddAlternateEnding::undo()
{
    score->RemoveAlternateEnding(altEnding);
}
