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
  
#include "removesystem.h"

#include <powertabdocument/score.h>

RemoveSystem::RemoveSystem(Score* score, quint32 index) :
    score(score),
    index(index)
{
    systemCopy = score->GetSystem(index);
    setText(QObject::tr("Remove System"));
}

void RemoveSystem::redo()
{
    score->RemoveSystem(index);
    emit triggered();
}

void RemoveSystem::undo()
{
    score->InsertSystem(systemCopy, index);
    emit triggered();
}
