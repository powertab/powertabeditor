#include "editrehearsalsign.h"

#include <powertabdocument/rehearsalsign.h>

EditRehearsalSign::EditRehearsalSign(RehearsalSign& rehearsalSign, bool isShown,
                                     uint8_t letter, const std::string& description) :
    rehearsalSign(rehearsalSign),
    isShown(isShown),
    letter(letter),
    description(description)
{
    if (!isShown)
    {
        setText(QObject::tr("Remove Rehearsal Sign"));
        this->letter = rehearsalSign.GetLetter();
        this->description = rehearsalSign.GetDescription();
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
        rehearsalSign.SetLetter(letter);
        rehearsalSign.SetDescription(description);
    }
    else
    {
        rehearsalSign.Clear();
    }
}
