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

#include "addartificialharmonic.h"

AddArtificialHarmonic::AddArtificialHarmonic(const ScoreLocation &location,
                                             const ArtificialHarmonic &harmonic)
    : QUndoCommand(QObject::tr("Add Artificial Harmonic")),
      myLocation(location),
      myHarmonic(harmonic)
{
}

void AddArtificialHarmonic::redo()
{
    myLocation.getNote()->setArtificialHarmonic(myHarmonic);
}

void AddArtificialHarmonic::undo()
{
    myLocation.getNote()->clearArtificialHarmonic();
}
