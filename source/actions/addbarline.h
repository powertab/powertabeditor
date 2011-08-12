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
  
#ifndef ADDBARLINE_H
#define ADDBARLINE_H

#include <QUndoCommand>
#include <memory>

class System;
class Barline;

class AddBarline : public QUndoCommand
{
public:
    AddBarline(std::shared_ptr<System> system, quint32 position, quint8 type, quint8 repeats);
    void redo();
    void undo();

protected:
    std::shared_ptr<System> system;
    const quint32 position;
    const quint8 type;
    const quint8 repeats;
    std::shared_ptr<Barline> newBar;
};

#endif // ADDBARLINE_H
