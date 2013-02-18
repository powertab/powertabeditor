/*
  * Copyright (C) 2013 Cameron White
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

#include "removemusicaldirection.h"

#include <powertabdocument/system.h>

RemoveMusicalDirection::RemoveMusicalDirection(boost::shared_ptr<System> system,
        boost::shared_ptr<Direction> direction) :
    QUndoCommand(QObject::tr("Remove Musical Direction")),
    system(system),
    direction(direction)
{
}

void RemoveMusicalDirection::redo()
{
    system->RemoveDirection(direction);
}

void RemoveMusicalDirection::undo()
{
    system->InsertDirection(direction);
}
