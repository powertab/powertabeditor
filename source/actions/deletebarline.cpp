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
  
#include "deletebarline.h"

#include <powertabdocument/barline.h>
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

DeleteBarline::DeleteBarline(Score* score, boost::shared_ptr<System> system,
                             boost::shared_ptr<Barline> barline) :
    QUndoCommand(QObject::tr("Delete Barline")),
    score(score),
    system(system),
    barline(barline)
{
}

void DeleteBarline::redo()
{
    system->RemoveBarline(barline->GetPosition());

    // Update the rehearsal signs letters, since a rehearsal sign may have been
    // removed.
    score->FormatRehearsalSigns();
}

void DeleteBarline::undo()
{
    system->InsertBarline(barline);
    score->FormatRehearsalSigns();
}
