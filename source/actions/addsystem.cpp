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

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <boost/make_shared.hpp>

AddSystem::AddSystem(Score* score, quint32 index) :
    score(score),
    index(index)
{
    setText(QObject::tr("Add System"));
}

void AddSystem::redo()
{
    boost::shared_ptr<System> newSystem = boost::make_shared<System>();

    // Carry over the time signature and key signature from the previous system
    // if possible.
    if (index > 0)
    {
        boost::shared_ptr<const System> prevSystem(score->GetSystem(index - 1));
        KeySignature key = prevSystem->GetEndBar()->GetKeySignature();
        TimeSignature time = prevSystem->GetEndBar()->GetTimeSignature();

        boost::shared_ptr<Barline> endBar = newSystem->GetEndBar();
        endBar->SetKeySignature(key);
        endBar->SetTimeSignature(time);

        boost::shared_ptr<Barline> startBar = newSystem->GetStartBar();
        key.SetShown(true);
        startBar->SetKeySignature(key);
        startBar->SetTimeSignature(time);
    }

    // adjust the location of the system (should be below the previous system)
    // TODO - move this into the Score::InsertSystem method (or add another method)??
    if (index != 0)
    {
        const Rect prevRect = score->GetSystem(index - 1)->GetRect();

        Rect currentRect = newSystem->GetRect();
        currentRect.SetTop(prevRect.GetBottom() + Score::SYSTEM_SPACING);
        newSystem->SetRect(currentRect);
    }

    score->InsertSystem(newSystem, index);

    emit triggered();
}

void AddSystem::undo()
{
    score->RemoveSystem(index);

    emit triggered();
}
