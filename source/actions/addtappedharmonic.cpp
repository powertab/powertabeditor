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
  
#include "addtappedharmonic.h"

#include <score/note.h>

AddTappedHarmonic::AddTappedHarmonic(const ScoreLocation &location,
                                     int tappedFret)
    : QUndoCommand(QObject::tr("Add Tapped Harmonic")),
      myLocation(location),
      myTappedFret(tappedFret)
{
}

void AddTappedHarmonic::redo()
{
    myLocation.getNote()->setTappedHarmonicFret(myTappedFret);
}

void AddTappedHarmonic::undo()
{
    myLocation.getNote()->clearTappedHarmonic();
}
