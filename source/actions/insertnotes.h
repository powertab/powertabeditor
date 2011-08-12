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
  
#ifndef INSERTNOTES_H
#define INSERTNOTES_H

#include <QUndoCommand>
#include <vector>
#include <memory>

class Position;
class System;
class Staff;

class InsertNotes : public QUndoCommand
{
public:
    InsertNotes(std::shared_ptr<System> system, std::shared_ptr<Staff> staff,
                uint32_t insertionPos, const std::vector<Position*>& newPositions);
    ~InsertNotes();

    void redo();
    void undo();

private:
    std::shared_ptr<System> system;
    std::shared_ptr<Staff> staff;
    const uint32_t insertionPos;
    std::vector<Position*> newPositions;

    std::shared_ptr<System> originalSystem;
    bool positionsInUse;
};

#endif // INSERTNOTES_H
