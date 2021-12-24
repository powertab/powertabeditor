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

#include "addpositionproperty.h"

AddPositionProperty::AddPositionProperty(const ScoreLocation &location,
                                         Position::SimpleProperty property,
                                         const QString &positionDescription)
    : QUndoCommand(tr("Set %1").arg(positionDescription)),
      myLocation(location),
      myProperty(property)
{
    for (const Position *pos : myLocation.getSelectedPositions())
        myOriginalPositions.push_back(*pos);
}

void AddPositionProperty::redo()
{
    for (Position *pos : myLocation.getSelectedPositions())
        pos->setProperty(myProperty, true);
}

void AddPositionProperty::undo()
{
    std::vector<Position *> selectedPositions = myLocation.getSelectedPositions();

    for (size_t i = 0; i < myOriginalPositions.size(); ++i)
        *selectedPositions[i] = myOriginalPositions[i];
}
