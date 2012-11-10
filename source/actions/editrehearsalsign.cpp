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
  
#include "editrehearsalsign.h"

#include <powertabdocument/rehearsalsign.h>
#include <powertabdocument/score.h>

EditRehearsalSign::EditRehearsalSign(Score* score, RehearsalSign& rehearsalSign,
                                     bool isShown, const std::string& description) :
    score(score),
    rehearsalSign(rehearsalSign),
    isShown(isShown),
    description(description)
{
    if (!isShown)
    {
        setText(QObject::tr("Remove Rehearsal Sign"));
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
        rehearsalSign.SetLetter('A'); // Set the rehearsal sign to be active.
        rehearsalSign.SetDescription(description);
    }
    else
    {
        rehearsalSign.Clear();
    }

    // Recompute the letters for all rehearsal signs.
    score->FormatRehearsalSigns();
}
