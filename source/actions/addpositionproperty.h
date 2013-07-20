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
  
#ifndef ACTIONS_ADDPOSITIONPROPERTY_H
#define ACTIONS_ADDPOSITIONPROPERTY_H

#include <QUndoCommand>
#include <score/position.h>
#include <score/scorelocation.h>

/// Sets a simple position property for each of the selected notes.
class AddPositionProperty : public QUndoCommand
{
public:
    AddPositionProperty(const ScoreLocation &location,
                        Position::SimpleProperty property,
                        const QString &positionDescription);

    virtual void redo();
    virtual void undo();

private:
    ScoreLocation myLocation;
    const Position::SimpleProperty myProperty;
};

#endif
