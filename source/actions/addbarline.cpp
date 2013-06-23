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
  
#include "addbarline.h"

#include <boost/make_shared.hpp>

#include <powertabdocument/barline.h>
#include <powertabdocument/system.h>

using boost::shared_ptr;

AddBarline::AddBarline(shared_ptr<System> system, quint32 position, quint8 type, quint8 repeats) :
    system(system),
    position(position),
    newBar(boost::make_shared<Barline>(position, type, repeats))
{
    setText(QObject::tr("Insert Barline"));

    // Use the same time signature and key signature from the previous bar.
    System::BarlineConstPtr prevBar(system->GetPrecedingBarline(position));

    KeySignature key = prevBar->GetKeySignature();
    key.SetShown(false);
    newBar->SetKeySignature(key);

    TimeSignature time = prevBar->GetTimeSignature();
    time.SetShown(false);
    newBar->SetTimeSignature(time);
}

void AddBarline::redo()
{
    system->InsertBarline(newBar);
}

void AddBarline::undo()
{
    system->RemoveBarline(position);
}
