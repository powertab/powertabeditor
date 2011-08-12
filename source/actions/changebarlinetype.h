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
  
#ifndef CHANGEBARLINETYPE_H
#define CHANGEBARLINETYPE_H

#include <QUndoCommand>
#include <memory>

class Barline;

// Modifys barline properties

class ChangeBarLineType : public QUndoCommand
{
public:
    ChangeBarLineType(std::shared_ptr<Barline> bar, quint8 barType, quint8 repeats);
    virtual void undo();
    virtual void redo();

private:
    std::shared_ptr<Barline> barLine;
    quint8 type;
    quint8 repeatCount;
    quint8 originalType;
    quint8 originalRepeatCount;
};

#endif // CHANGEBARLINETYPE_H
