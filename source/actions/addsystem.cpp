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
  
#include "addsystem.h"

#include <boost/make_shared.hpp>

#include <powertabdocument/barline.h>
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

AddSystem::AddSystem(Score* score, quint32 index) :
    QUndoCommand(QObject::tr("Add System")),
    score(score),
    index(index),
    system(boost::make_shared<System>())
{
    // Carry over the time signature and key signature from the previous system
    // if possible.
    if (index > 0)
    {
        boost::shared_ptr<const System> prevSystem(score->GetSystem(index - 1));
        KeySignature key = prevSystem->GetEndBar()->GetKeySignature();
        TimeSignature time = prevSystem->GetEndBar()->GetTimeSignature();

        boost::shared_ptr<Barline> endBar = system->GetEndBar();
        endBar->SetKeySignature(key);
        endBar->SetTimeSignature(time);

        boost::shared_ptr<Barline> startBar = system->GetStartBar();
        key.SetShown(true);
        startBar->SetKeySignature(key);
        startBar->SetTimeSignature(time);
    }

    // adjust the location of the system (should be below the previous system)
    // TODO - move this into the Score::InsertSystem method (or add another method)??
    if (index != 0)
    {
        const Rect prevRect = score->GetSystem(index - 1)->GetRect();

        Rect currentRect = system->GetRect();
        currentRect.SetTop(prevRect.GetBottom() + Score::SYSTEM_SPACING);
        system->SetRect(currentRect);
    }
}

void AddSystem::redo()
{
    score->InsertSystem(system, index);
}

void AddSystem::undo()
{
    score->RemoveSystem(index);
}
