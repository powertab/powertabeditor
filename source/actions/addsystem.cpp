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

#include <score/score.h>

AddSystem::AddSystem(Score &score, int index)
    : QUndoCommand(QObject::tr("Add System")),
      myScore(score),
      myIndex(index)
{
}

void AddSystem::redo()
{
    // Set up a default staff.
    System system;
    system.insertStaff(Staff());

    // Carry over the time signature and key signature from the previous system
    // if possible.
    if (myIndex > 0)
    {
        const System &prevSystem = myScore.getSystems()[myIndex - 1];
        KeySignature key = prevSystem.getBarlines().back().getKeySignature();
        TimeSignature time = prevSystem.getBarlines().back().getTimeSignature();

        Barline &endBar = system.getBarlines().back();
        key.setVisible(false);
        endBar.setKeySignature(key);
        endBar.setTimeSignature(time);

        Barline &startBar = system.getBarlines().front();
        key.setVisible(true);
        startBar.setKeySignature(key);
        startBar.setTimeSignature(time);
    }

    myScore.insertSystem(system, myIndex);
}

void AddSystem::undo()
{
    myScore.removeSystem(myIndex);
}
