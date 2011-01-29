#include "editrehearsalsign.h"

#include <powertabdocument/rehearsalsign.h>
#include <powertabdocument/score.h>

EditRehearsalSign::EditRehearsalSign(Score* score, System* system, RehearsalSign* rehearsalSign, bool isShown, quint8 letter, std::string description) :
    score(score),
    system(system),
    rehearsalSign(rehearsalSign),
    isShown(isShown),
    letter(letter),
    description(description)
{
    if (!isShown)
    {
        setText(QObject::tr("Remove Rehearsal Sign"));
        this->letter = rehearsalSign->GetLetter();
        this->description = rehearsalSign->GetDescription();
    }
    else
    {
        setText(QObject::tr("Rehearsal Sign"));
    }
}

void EditRehearsalSign::redo()
{
    showHide(isShown);
}

void EditRehearsalSign::undo()
{
    showHide(!isShown);
}

void EditRehearsalSign::showHide(bool show)
{
    if (show)
    {
        rehearsalSign->SetLetter(letter);
        rehearsalSign->SetDescription(description);
    }
    else
    {
        rehearsalSign->Clear();
    }

    score->UpdateSystemHeight(system);
}
