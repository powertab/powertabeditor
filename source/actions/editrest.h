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
  
#ifndef EDITREST_H
#define EDITREST_H

#include <QUndoCommand>

class Position;

class EditRest : public QUndoCommand
{
public:
    EditRest(Position* position, uint8_t duration);

    void redo();
    void undo();

private:
    Position* position;
    const uint8_t newDuration;
    const uint8_t originalDuration;
    const bool wasAlreadyRest;
};

#endif // EDITREST_H
